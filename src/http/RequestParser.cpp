#include "http/RequestParser.hpp"

#include "shared/stringUtils.hpp"

// todo error handling

namespace http {

	const char RequestParser::TCHAR[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ', '!', 0, '#', '$', '%', '&', '\'', 0, 0, '*', '+', 0, '-', '.', 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 0, '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, '|', 0, '~', 0}; // the formatter is shit pardon me
	const char RequestParser::WHITESPACE[] = " \t";

	RequestParser::RequestParser()
		: m_req(new Request())
		, m_state(START) {
	}

	RequestParser::~RequestParser() {
		delete m_req;
	}

	// note: this function expects the caller to fill the write buffer with data before each call
	void RequestParser::process() {
		this->clearPendingState();

		try {
			this->parse();
		} catch (const http::exception& e) { // avoid reconstructing exception
			throw e;
		} catch (const std::exception& e) {
			throw http::exception(INTERNAL_SERVER_ERROR, e.what());
		}
	}

	void RequestParser::parse() {
		while (this->isComplete() != true && this->isPending() != true) {
			if (m_state == START) {
				this->parseRequestLine();
			} else if (m_state == HEADERS) {
				this->parseHeaders();
			} else if (m_state == BODY) {
				this->parseBody();
			} else if (m_state == CHUNK_SIZE) {
				this->parseChunkSize();
			} else if (m_state == CHUNK_DATA) {
				this->parseChunkData();
			}
		}
	}

	void RequestParser::parseRequestLine() {
		char* line = this->readLine();
		if (!line) {
			return;
		}

		std::size_t size = std::strcspn(line, WHITESPACE);
		m_req->setMethod(line, size);
		line += size + 1;

		size = std::strcspn(line, WHITESPACE);
		m_req->setUri(line, size);
		line += size + 1;

		size = std::strcspn(line, WHITESPACE);
		m_req->setVersion(line, size);
		line += size;

		if (*line) {
			throw http::exception(BAD_REQUEST, "malformed request line: expected seperation of exactly 2 spaces");
		}

		m_state = HEADERS;
	}

	void RequestParser::parseHeaders() {
		while (true) {
			char* line = this->readLine();
			if (!line || !*line) {
				break;
			}
			Token key = this->extractHeaderKey(line);
			while (*line) {
				Token value = this->extractHeaderValue(line);
				m_req->setHeader(key.token, key.size, value.token, value.size);
			}
		}

		// todo: check for trailing headers
		if (m_req->hasHeader("content-length")) {
			this->setState(BODY);
		} else if (m_req->isChunked()) { // todo send error if transfer but not chunked
			this->setState(CHUNK_SIZE);
		} else {
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
		return Token(value, size);
	}

	void RequestParser::parseBody() {
		if (!m_contentLength) {
			if (m_req->getHeader("content-length").size() != 1) {
				throw http::exception(BAD_REQUEST, "invalid content-length: too many values");
			}

			try {
				m_contentLength = shared::string::toNum<std::size_t>(m_req->getHeader("content-length").at(0), 10);
				if (m_contentLength == 0) {
					this->setState(COMPLETE);
					return;
				}
			} catch (const std::exception& e) {
				throw http::exception(BAD_REQUEST, "invalid content-length: could not parse: " + std::string(e.what()));
			}
		}

		size_t available = std::min(m_buffer.size(), m_contentLength);
		if (available == 0) {
			this->setPendingState();
			return;
		}

		m_req->appendToBody(m_buffer.getReadPos(), available);
		m_buffer.consume(available);
		m_contentLength -= available;

		if (m_contentLength == 0) {
			this->setState(COMPLETE);
			std::cout << m_buffer.size() << std::endl;
		} else {
			this->setPendingState();
		}
	}

	void RequestParser::parseChunkSize() {
		const char* line = this->readLine();
		if (!line) {
			return;
		}

		try {
			m_chunkSize = shared::string::toNum<std::size_t>(line, 16);
		} catch (const std::exception& e) {
			throw http::exception(BAD_REQUEST, "Invalid chunk size: could not parse.");
		}

		if (m_chunkSize == 0) {
			this->setState(COMPLETE);
		} else {
			this->setState(CHUNK_DATA);
		}
	}

	void RequestParser::parseChunkData() {
		size_t available = std::min(m_buffer.size(), m_chunkSize);
		if (available == 0) {
			this->setPendingState();
			return;
		}

		m_req->appendToBody(m_buffer.getReadPos(), available);
		m_buffer.consume(available);
		m_chunkSize -= available;

		if (m_chunkSize == 0) {
			if (m_buffer.size() < CRLF.length()) {
				this->setPendingState();
				return;
			}
			m_buffer.consume(CRLF.length());
			this->setState(CHUNK_SIZE);
		} else {
			this->setPendingState();
		}
	}

	// this is actually making us read the data twice
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

	void RequestParser::reset() {
		m_buffer.reset();
		delete m_req;
		m_req = new Request();
		this->setState(START);
	}

	Request* RequestParser::releaseRequest() {
		Request* released = m_req;
		m_req = NULL;
		return released;
	}

	bool RequestParser::isComplete() const {
		return m_state == COMPLETE;
	}

	bool RequestParser::isPending() const {
		return m_state & PENDING_MASK;
	}

	shared::Buffer<BUFFER_SIZEE>& RequestParser::getWriteBuffer() {
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