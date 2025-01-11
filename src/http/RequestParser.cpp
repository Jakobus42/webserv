#include "http/RequestParser.hpp"

#include <cstring>

namespace http {

	RequestParser::RequestParser()
		: m_req()
		, m_readPos(0)
		, m_writePos(0)
		, m_state(STATE_START) {
		std::memset(m_buffer, 0, BUFFER_SIZEEEE);
	}

	RequestParser::~RequestParser() {}

	void RequestParser::feed(const char* data, std::size_t size) {

		if (m_writePos + size > BUFFER_SIZEEEE) {
			this->compactBuffer();
			if (m_writePos + size > BUFFER_SIZEEEE) {
				throw std::length_error("buffer overflow");
			}
		}

		memcpy(m_buffer + m_writePos, data, size);
		m_writePos += size;

		this->parse();
	}

	void RequestParser::parse() {
		while (m_readPos != m_writePos && this->isComplete() != true) {
			if (m_state == STATE_START) {
				this->parseRequestLine();
			} else if (m_state == STATE_HEADERS) {
				this->parseHeaders();
			} else if (m_state == STATE_BODY) {
				this->parseBody();
			} else if (m_state == STATE_CHUNK_SIZE) {
				this->parseChunkSize();
			} else if (m_state == STATE_CHUNK_DATA) {
				this->parseChunkData();
			}
		}
	}

	void RequestParser::parseRequestLine() {
		const char* line = this->readLine();
		if (line == NULL) {
			throw std::runtime_error("invalid request line");
		}
		std::cout.write(line, strlen(line));

		m_state = STATE_HEADERS;
	}

	void RequestParser::parseHeaders() {
		m_state = STATE_BODY;
	}

	void RequestParser::parseBody() {
		m_state = STATE_COMPLETE;
	}

	void RequestParser::parseChunkSize() {
		m_state = STATE_CHUNK_DATA;
	}

	void RequestParser::parseChunkData() {
		m_state = STATE_COMPLETE;
	}

	const char* RequestParser::readLine() {
		char* lineEnd = findCRLF();
		if (lineEnd == NULL) {
			return NULL;
		}

		const char* line = &m_buffer[m_readPos];
		*lineEnd = '\0';

		m_readPos = lineEnd - m_buffer + 2;
		return line;
	}

	char* RequestParser::findCRLF() {
		for (std::size_t i = m_readPos; i < m_writePos; ++i) {
			if (strncmp(&m_buffer[i], CRLF.c_str(), CRLF.length()) == 0) {
				return &m_buffer[i];
			}
		}
		return NULL;
	}

	void RequestParser::compactBuffer() {
		std::size_t unreadSize = m_writePos - m_readPos;
		if (unreadSize > 0) {
			memmove(m_buffer, m_buffer + m_readPos, unreadSize);
		}
		m_writePos = unreadSize;
		m_readPos = 0;
	}

	void RequestParser::reset() {
		m_state = STATE_START;
		m_req = Request();
		m_readPos = 0;	// for now
		m_writePos = 0; // for now
	}

	bool RequestParser::isComplete() const { return m_state == STATE_COMPLETE; }

	const Request& RequestParser::getRequest() const { return m_req; }

} /* namespace http */