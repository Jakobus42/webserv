#include "http/RequestParser.hpp"

#include "shared/stringUtils.hpp"

// TODO error handling

namespace http {

	const char RequestParser::TCHAR[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ' ', '!', 0, '#', '$', '%', '&', '\'', 0, 0, '*', '+', 0, '-', '.', 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, 0, 0, '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, '|', 0, '~', 0}; // the formatter is shit pardon me
	const char RequestParser::WHITESPACE[] = " \t";

	const int RequestParser::PENDING_MASK = 0x1000;

	RequestParser::RequestParser()
		: m_req(new Request())
		, m_state(START) {
	}

	RequestParser::~RequestParser() {
		delete m_req;
	}

	void RequestParser::process() {
		this->clearPendingState();

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

	// RFC 7230 Section 3.1.1
	// note: method, target and version must be seperated by a single space.
	// missing: uri decoding
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

		if (*line != '\0') {
			throw std::runtime_error("invalid request line format");
		}
		m_state = HEADERS;
	}

	// todo: check for trailing headers

	// RFC 7230 Section 3.2
	// field-name ":" OWS field-value OWS
	void RequestParser::parseHeaders() {
		while (char* line = this->readLine()) {
			Token key = this->extractHeaderKey(line);
			while (*line) {
				Token value = this->extractHeaderValue(line);
				m_req->setHeader(key.token, key.size, value.token, value.size);
				break;
			}
		}

		if (m_req->hasHeader("Content-Length")) {
			this->setState(BODY);
		} else if (m_req->isChunked()) {
			this->setState(CHUNK_SIZE);
		} else {
			this->setState(COMPLETE);
		}
	}

	RequestParser::Token RequestParser::extractHeaderKey(char*& line) {
		std::size_t keySize = 0;
		for (; *line && *line != ':'; ++line) {
			if (this->isTChar(*line) != true) {
				throw std::runtime_error("invalid char found in header name field: expected tchar");
			}
			keySize++;
		}
		if (*line != ':' || std::strchr(WHITESPACE, *(line - 1))) {
			throw std::runtime_error("invalid header name format");
		}
		return Token(line++ - keySize, keySize);
	}

	RequestParser::Token RequestParser::extractHeaderValue(char*&) {
		return Token();
	}

	void RequestParser::parseBody() {
		if (m_req->getHeader("Content-Length").size() != 1) {
			throw std::runtime_error("invalid Content-Length: too many values");
		}

		const std::string& contentLength = m_req->getHeader("Content-Length").at(0);
		std::size_t len = shared::string::toNum<std::size_t>(contentLength, 10);
		if (m_buffer.size() < len) {
			this->setPendingState();
			return;
		}

		m_req->setBody(std::string(m_buffer.getReadPos(), len));
		m_buffer.consume(len);

		this->setState(COMPLETE);
	}

	void RequestParser::parseChunkSize() {
		const char* line = this->readLine();
		if (!line) {
			return;
		}

		m_chunkSize = shared::string::toNum<std::size_t>(line, 16);
		if (m_chunkSize == 0) {
			this->setState(COMPLETE);
		} else {
			this->setState(CHUNK_DATA);
		}
	}

	void RequestParser::parseChunkData() {
		if (m_buffer.size() < m_chunkSize) {
			this->setPendingState();
			return;
		}

		m_req->appendToBody(m_buffer.getReadPos(), m_chunkSize);
		m_buffer.consume(m_chunkSize + CRLF.length());

		this->setState(CHUNK_SIZE);
	}

	char* RequestParser::readLine() {
		char* line = m_buffer.getReadPos();

		char* lineEnd = strstr(line, CRLF.c_str());
		if (lineEnd == NULL) {
			this->setPendingState();
			return NULL;
		}

		*lineEnd = '\0';
		std::size_t consumed = line == lineEnd ? 0 : lineEnd + CRLF.size() - line;
		m_buffer.consume(consumed);
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

	shared::Buffer<BUFFER_SIZE>& RequestParser::getWriteBuffer() {
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