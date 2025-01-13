#include "http/RequestParser.hpp"
#include "shared/stringUtils.hpp"

#include <algorithm>
#include <cstring>

// TODO: better error handling (so shit right now)
// TODO: more error handling

namespace http {

	RequestParser::RequestParser()
		: m_req()
		, m_buffer()
		, m_state(START)
		, m_chunkSize(0) {
	}

	RequestParser::~RequestParser() {}

	void RequestParser::feed(const char* data, std::size_t size) {
		m_buffer.append(data, size);
		this->parse();
	}

	void RequestParser::parse() {
		while (m_buffer.size() != 0 && this->isComplete() != true) {
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
		const char* line = this->readLine();

		m_req.setMethod(stringToMethod(nextToken(line, ' '))); // so much copying:c
		m_req.setUri(nextToken(line, ' '));
		m_req.setVersion(nextToken(line, ' '));

		m_state = HEADERS;
	}

	void RequestParser::parseHeaders() {
		while (true) {
			const char* line = this->readLine();
			if (*line == '\0') {
				break;
			}
			this->parseHeader(line);
		}

		if (m_req.hasHeader("Content-Length")) {
			m_state = BODY;
		} else if (m_req.isChunked()) {
			m_state = CHUNK_SIZE;
		} else {
			m_state = COMPLETE;
		}
	}

	void RequestParser::parseHeader(const char* line) {
		std::string key = this->nextToken(line, ':');
		if (*line == '\0' && *(line - 1) != ':') {
			throw std::runtime_error("invalid header format: header missing ':'");
		}

		while (*line) {
			std::string value = this->nextToken(line, ',');
			m_req.setHeader(key, value);
		}
	}

	void RequestParser::parseBody() {
		if (m_req.getHeader("Content-Length").size() != 1) {
			throw std::runtime_error("invalid Content-Length: too many values");
		}

		std::size_t length = shared::string::convertToNumber<std::size_t>(m_req.getHeader("Content-Length").at(0), 10);
		if (m_buffer.size() < length) { // TODO: handle somehow (wait for more data)
			throw std::runtime_error("invalid Content-Length: too large body");
		}

		m_req.setBody(std::string(m_buffer.getReadPos(), length)); // TODO copy:c
		m_buffer.consume(length);

		m_state = COMPLETE;
	}

	void RequestParser::parseChunkSize() {
		const char* line = this->readLine();

		m_chunkSize = shared::string::convertToNumber<std::size_t>(line, 16);
		if (m_chunkSize == 0) {
			m_state = COMPLETE;
		} else {
			m_state = CHUNK_DATA;
		}
	}

	void RequestParser::parseChunkData() {
		if (m_buffer.size() < m_chunkSize) {
			return; // TODO: handle somehow
		}

		m_req.appendToBody(std::string(m_buffer.getReadPos(), m_chunkSize));
		m_buffer.consume(m_chunkSize);

		this->readLine();

		m_state = CHUNK_SIZE;
	}

	// TODO: handle quotes and avoid copy
	// strtok?
	std::string RequestParser::nextToken(const char*& line, char delimiter) {
		while (*line && std::isspace(*line)) {
			++line;
		}

		const char* start = line;
		while (*line && *line != delimiter) {
			++line;
		}
		const char* end = line;

		while (end > start && std::isspace(*(end - 1))) {
			--end;
		}

		std::string token(start, end);
		line += *line == delimiter ? 1 : 0;
		return token;
	}

	const char* RequestParser::readLine() {
		char* line = m_buffer.getReadPos();

		char* lineEnd = strstr(line, CRLF.c_str());
		if (lineEnd == NULL) {
			throw std::runtime_error("readline: no CRLF found");
		}

		*lineEnd = '\0';
		m_buffer.consume(lineEnd - line + CRLF.size());
		return line;
	}

	void RequestParser::reset() {
		m_state = START;
		m_req = Request();
		m_buffer.compact();
	}

	bool RequestParser::isComplete() const { return m_state == COMPLETE; }

	const Request& RequestParser::getRequest() const { return m_req; }

} /* namespace http */