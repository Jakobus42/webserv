#include "http/RequestParser.hpp"

#include "shared/stringUtils.hpp"

namespace http {

	const char RequestParser::TCHAR[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ', '!', 0, '#', '$', '%', '&', '\'', 0, 0, '*', '+', 0, '-', '.', 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 0, '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, '|', 0, '~', 0}; // the formatter is shit pardon me
	const char RequestParser::WHITESPACE[] = " \t";

	RequestParser::RequestParser()
		: m_req()
		, m_contentLength(0)
		, m_state(START) {
	}

	RequestParser::~RequestParser() {}

	void RequestParser::process() {
		this->clearPendingState();

		try {
			this->parse();
		} catch (const http::exception& e) {
			m_req.setStatusCode(e.getCode());
			this->setState(ERROR);
		} catch (const std::exception& e) {
			m_req.setStatusCode(INTERNAL_SERVER_ERROR);
			this->setState(ERROR);
		}
	}

	void RequestParser::parse() {
		while (this->isComplete() != true && this->isPending() != true) {
			if (m_state == START) {
				this->parseRequestLine();
			} else if (m_state == HEADERS || m_state == TRAILING_HEADERS) {
				this->parseHeaders();
			} else if (m_state == CHUNK_SIZE) {
				this->parseChunkSize();
			} else if (m_state == BODY || m_state == CHUNK_DATA) {
				this->parseData();
			}
		}
	}

	void RequestParser::parseRequestLine() {
		char* line = this->readLine();
		if (!line) {
			return;
		}

		std::size_t size = std::strcspn(line, WHITESPACE);
		m_req.setMethod(line, size);
		line += size + 1;

		size = std::strcspn(line, WHITESPACE);
		if (size > MAX_URI_LENGTH) {
			throw http::exception(URI_TOO_LONG, "URI too large");
		}
		m_req.setUri(line, size);
		line += size + 1;

		size = std::strcspn(line, WHITESPACE);
		m_req.setVersion(line, size);
		line += size;

		if (*line) {
			throw http::exception(BAD_REQUEST, "malformed request line: expected seperation of exactly 2 spaces");
		}

		m_state = HEADERS;
	}

	void RequestParser::parseHeaders() {
		while (true) {
			char* line = this->readLine();
			if (!line) {
				return;
			} else if (!*line) {
				break;
			}

			Token key = this->extractHeaderKey(line);
			if (!*line) {
				m_req.setHeader(key.token, key.size, NULL, 0);
			}
			std::size_t valCount = 0;
			while (*line) {
				Token value = this->extractHeaderValue(line);
				m_req.setHeader(key.token, key.size, value.token, value.size);
				if (++valCount > MAX_HEADER_VALUE_COUNT) {
					throw http::exception(PAYLOAD_TOO_LARGE, "header value count too large");
				}
			}
			if (m_req.getHeaders().size() > MAX_HEADER_COUNT) {
				throw http::exception(PAYLOAD_TOO_LARGE, "header count too large");
			}
		}
		if (m_state == HEADERS) {
			this->interpretHeaders();
		} else if (m_state == TRAILING_HEADERS) {
			this->setState(COMPLETE);
		}
	}

	RequestParser::Token RequestParser::extractHeaderKey(char*& line) {
		std::size_t keySize = 0;
		for (; *line && *line != ':'; ++line) {
			if (this->isTChar(*line) != true) {
				throw http::exception(BAD_REQUEST, "malformed header name: expected tchar");
			}
			*line = std::tolower(*line);
			keySize++;
		}

		if (keySize == 0) {
			throw http::exception(BAD_REQUEST, "malformed header name: expected size > 0");
		} else if (*line != ':') {
			throw http::exception(BAD_REQUEST, "malformed header name: missing colon separator");
		} else if (std::strchr(WHITESPACE, *(line - 1))) {
			throw http::exception(BAD_REQUEST, "malformed header name: no whitespace allowed before colon");
		}

		if (keySize > MAX_HEADER_NAME_LENGTH) {
			throw http::exception(PAYLOAD_TOO_LARGE, "header key too large");
		}

		return Token(line++ - keySize, keySize);
	}

	// empty values are allowed.
	RequestParser::Token RequestParser::extractHeaderValue(char*& line) {
		const char* value = line + std::strspn(line, WHITESPACE);
		std::size_t raw_size = std::strcspn(value, ",");

		std::size_t size = raw_size;
		while (size > 0 && std::strchr(WHITESPACE, value[size - 1])) {
			--size;
		}

		line = const_cast<char*>(value + raw_size);
		if (*line == ',') {
			++line;
		}

		if (size > MAX_HEADER_VALUE_LENGTH) {
			throw http::exception(PAYLOAD_TOO_LARGE, "header value length too large");
		}
		return Token(value, size);
	}

	void RequestParser::interpretHeaders() {
		if (m_req.hasHeader("content-length") && m_req.hasHeader("transfer-encoding")) {
			throw http::exception(BAD_REQUEST, "invalid header combination: cant have content-length and transfer-encoding");
		}

		if (m_req.hasHeader("content-length")) {
			this->validateContentLength();
			this->setState(m_contentLength == 0 ? COMPLETE : BODY);
		} else if (m_req.hasHeader("transfer-encoding")) {
			this->validateTransferEncoding();
			this->setState(CHUNK_SIZE);
		} else {
			this->setState(COMPLETE);
		}
	}

	void RequestParser::parseData() {
		if (m_contentLength) {
			size_t available = std::min(m_buffer.size(), m_contentLength);
			if (available == 0) {
				this->setPendingState();
				return;
			}

			m_req.appendToBody(m_buffer.getReadPos(), available);
			if (m_req.getBody().size() > MAX_BODY_SIZE) {
				throw http::exception(PAYLOAD_TOO_LARGE, "body too large");
			}
			m_buffer.consume(available);
			m_contentLength -= available;
			if (m_contentLength > 0) {
				this->setPendingState();
				return;
			}
		}


		if (m_state == BODY) {
			this->setState(COMPLETE);
		} else if (m_state == CHUNK_DATA) {
			if (m_buffer.size() < CRLF.length()) {
				this->setPendingState();
			} else {
				m_buffer.consume(CRLF.length());
				this->setState(CHUNK_SIZE);
			}
		}
	}

	void RequestParser::parseChunkSize() {
		const char* line = this->readLine();
		if (!line) {
			return;
		}

		try {
			m_contentLength = shared::string::toNum<std::size_t>(line, 16);
		} catch (const std::exception& e) {
			throw http::exception(BAD_REQUEST, "invalid chunk size: could not parse.");
		}
		if (m_contentLength > MAX_BODY_SIZE) {
			throw http::exception(PAYLOAD_TOO_LARGE, "chunk size too large");
		}

		if (m_contentLength == 0) {
			this->setState(TRAILING_HEADERS);
		} else {
			this->setState(CHUNK_DATA);
		}
	}

	char* RequestParser::readLine() {
		char* line = m_buffer.getReadPos();

		char* lineEnd = strstr(line, CRLF.c_str());
		if (lineEnd == NULL) {
			this->setPendingState();
			return NULL;
		}

		*lineEnd = '\0';
		m_buffer.consume(lineEnd + CRLF.length() - line);
		return line;
	}

	void RequestParser::validateContentLength() {
		const std::vector<std::string>& contentLengthVec = m_req.getHeader("content-length");
		if (contentLengthVec.size() != 1) {
			throw http::exception(BAD_REQUEST, "malformed content-length: expected exactly one value");
		}

		try {
			m_contentLength = shared::string::toNum<std::size_t>(contentLengthVec.at(0), 10);
		} catch (const std::exception& e) {
			throw http::exception(BAD_REQUEST, "invalid content-length: could not parse: " + std::string(e.what()));
		}
		if (m_contentLength > MAX_BODY_SIZE) {
			throw http::exception(PAYLOAD_TOO_LARGE, "content-length too large");
		}
	}

	void RequestParser::validateTransferEncoding() {
		const std::vector<std::string>& transferEncodingVec = m_req.getHeader("transfer-encoding");
		if (transferEncodingVec.size() != 1) {
			throw http::exception(BAD_REQUEST, "malformed transfer-encoding: expected exactly one value");
		}

		const std::string& encoding = transferEncodingVec.at(0);
		if (encoding != "chunked") {
			throw http::exception(NOT_IMPLEMENTED, "transfer-encoding " + encoding + " is not implemented");
		}
	}

	void RequestParser::reset() {
		m_buffer.reset();
		this->setState(START);
	}

	const Request& RequestParser::getRequest() {
		return m_req;
	}

	bool RequestParser::isComplete() const {
		return m_state == COMPLETE;
	}

	bool RequestParser::isPending() const {
		return m_state & PENDING_MASK;
	}

	bool RequestParser::hasError() const {
		return m_state & ERROR;
	}

	shared::Buffer<REQUEST_BUFFER_SIZE>& RequestParser::getWriteBuffer() {
		return m_buffer;
	}

	bool RequestParser::isTChar(char c) const {
		return TCHAR[static_cast<unsigned char>(c)] != 0;
	}

	void RequestParser::setPendingState() {
		m_state |= PENDING_MASK;
	}

	void RequestParser::clearPendingState() {
		m_state &= ~PENDING_MASK;
	}

	void RequestParser::setState(ParseState state) {
		m_state = (m_state & PENDING_MASK) | state;
	}

} /* namespace http */