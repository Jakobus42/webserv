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
			if (m_chunkedStatus == CHUNK_SIZE) { // reading chunk size and chunk extentions
				if (!parseBodyChunkedSize(input)) {
					return false;
				}
				if (m_chunkedStatus == CHUNK_SIZE) {
					return true;
				}
			}
			if (m_chunkedStatus == CHUNK_DATA) { // reading chunk data
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
			if (m_chunkedStatus == CHUNK_DATA_END) { // skipping the \r\n after chunk
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

	bool Request::parseBodyChunkedSize(std::string& input) {
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
				if (line != "") { // TODO: chunk extension
					if (!parseChunkExtentions(line)) {
						return false;
					}
				}
			}
		}
		return true;
	}

	void removeWhitespaces(std::string& line) {
		while (line[0] == ' ' || line[0] == '\t') {
			line = line.substr(1);
		}
	}

	bool Request::parseChunkExtentions(std::string& line) {
		std::string key;
		std::string value;
		t_chunkedExtension chunkedExtension;
		chunkedExtension.start = m_requestData.body.size();
		chunkedExtension.end = chunkedExtension.start + m_contentLength;
		while (line != "") {
			key = "";
			value = "";
			bool hasValue = false;
			removeWhitespaces(line);
			if (line[0] == ';') // find next chunked extention
				line = line.substr(1);
			else {
				LOG("Error: Invalid chunked extention, \";\" expected after chunk size or other chunked extention", 1);
				return false;
			}
			removeWhitespaces(line);
			while (line != "" && line[0] != ' ' && line[0] != '\t' && line[0] != '=' && line[0] != ';') { // find Key
				key += line[0];
				if (key.length() > 100) {
					LOG("Error: Invalid chunked extention, key too long", 1);
					return false;
				}
				line = line.substr(1);
			}
			removeWhitespaces(line);
			if (line[0] == '=') {
				line = line.substr(1);
				hasValue = true;
				removeWhitespaces(line);
				while (line != "" && line[0] != ' ' && line[0] != '\t' && line[0] != ';') { // find value
					value += line[0];
					if (value.length() > 1000) {
						LOG("Error: Invalid chunked extention, value too long", 1);
						return false;
					}
					line = line.substr(1);
				}
			}
			if (key == "") {
				LOG("Error: Invalid chunked extention, (non-empty) key expected", 1);
				return false;
			}
			if (hasValue && value == "") {
				LOG("Error: Invalid chunked extention, (non-empty) value expected", 1);
				return false;
			}
			chunkedExtension.extensions[key] = value; // save that chunked extention data
			if (chunkedExtension.extensions.size() > 1000) {
				LOG("Error: Invalid chunked extention, too many extensions", 1);
				return false;
			}
		}
		m_chunkedExtensions.push_back(chunkedExtension);
		return true;
	}

} /* namespace http */