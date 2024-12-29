#include "http/Request.hpp"

namespace http {

	bool Request::parseBody(std::string& input) {
		if (m_expectedBody == NO_BODY) {
			m_status = PARSE_END;
			return true; // error if body exists?
		}
		if (m_expectedBody == CHUNKED) {
			return parseBodyChunked(input);
		}
		if (m_expectedBody == CONTENT_LENGTH) {
			if (m_requestData.body.size() + input.size() < m_contentLength) {
				m_requestData.body += input;
				input = "";
			} else {
				m_requestData.body += input.substr(0, m_contentLength - m_requestData.body.size());
				input = input.substr(m_contentLength - m_requestData.body.size());
				m_status = PARSE_END;
			}
		}
		return true;
	}

	bool Request::parseBodyChunked(std::string& input) {
		while (1) {
			if (m_chunkedStatus == CHUNK_SIZE) {
				std::string line;
				GetLineStatus status = getNextLineHTTP(input, line);
				if (status == GET_LINE_ERROR) {
					return false;
				}
				if (status == GET_LINE_OK) {
					if (line == "") {
						LOG("Error: Empty line in chunked encoding", 1);
						return false;
					} else {
						int ret = 0;
						m_contentLength = shared::string::StoiHex(line, ret);
						if (ret == -1) {
							LOG("Error: Invalid hexadecimal number in chunked encoding", 1);
							return false;
						}
						if (m_contentLength == 0) {
							m_chunkedStatus = CHUNK_END;
						} else {
							m_chunkedStatus = CHUNK_DATA;
						}
						// if (line != "") { //TODO: chunk extension
					}
				}
				if (m_chunkedStatus == CHUNK_SIZE) {
					return true;
				}
			}
			if (m_chunkedStatus == CHUNK_DATA) {
				std::string chunk = input.substr(0, m_contentLength);
				m_contentLength -= chunk.size();
				m_requestData.body += chunk;
				input = input.substr(chunk.size());
				if (m_contentLength == 0) {
					m_chunkedStatus = CHUNK_DATA_END;
				} else {
					return true;
				}
			}
			if (m_chunkedStatus == CHUNK_DATA_END) {
				if (input.length() < 2) {
					return true;
				}
				if (input[0] == '\r' && input[1] == '\n') {
					input = input.substr(2);
					m_chunkedStatus = CHUNK_SIZE;
				} else {
					LOG("Error: Invalid chunked encoding, \r\n expected after chunk.", 1);
					return false;
				}
			}
			if (m_chunkedStatus == CHUNK_END) {
				return parseHeaders(input, TRAILING);
			}
		}
	}

} /* namespace http */