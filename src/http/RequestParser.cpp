#include "http/Request.hpp"

namespace http {

	bool Request::parseHeader(std::string& line, HeaderType type) {
		bool last_flag = false;
		QuoteFlag qf = NO_QUOTE;
		std::string key = "";
		std::vector<std::string> values;
		unsigned long promptStart = 0;
		for (unsigned long i = 0; i < line.size(); i++) {
			// quote Management
			if ((line[i] == '\'' || line[i] == '\"') && qf == NO_QUOTE) {
				if (line[i] == '\'') {
					qf = SINGLE_QUOTE;
				} else {
					qf = DOUBLE_QUOTE;
				}
			}
			if ((line[i] == '\'' && qf == SINGLE_QUOTE) || (line[i] == '\"' && qf == DOUBLE_QUOTE)) {
				qf = NO_QUOTE;
			}
			// find key
			if (line[i] == ':' && qf == NO_QUOTE && key == "") {
				if (i == 0) {
					LOG("Request: Error: Empty key in header", 1);
					return false;
				}
				if (shared::string::isGreyspace(line[i - 1])) {
					LOG("Request: Error: Found whitespace before colon in the header section", 1);
					return false;
				}
				key = line.substr(0, i);
				promptStart = i + 1;
			}
			if ((line[i] == ',' && qf == NO_QUOTE) || i == line.size() - 1) {
				if (i == line.size() - 1) {
					if (line[i] != ',') {
						last_flag = true;
					}
					i++;
				}
				if (key == "") {
					LOG("Request: Error: Found value before key", 1);
					return false;
				}
				if (i == promptStart) {
					LOG("Request: Error: Empty value in header", 1);
					return false;
				}
				if (shared::string::isGreyspace(line[i - 1])) {
					LOG("Request: Error: Found whitespace before comma in the header section", 1);
					return false;
				}
				while (shared::string::isGreyspace(line[promptStart])) {
					promptStart++;
				}
				std::string val = line.substr(promptStart, i - promptStart);
				values.push_back(val);
				promptStart = i + 1;
			}
		}
		if (key == "") {
			LOG("Request: Error: No key found in header", 1);
			return false;
		}
		if (values.size() == 0) {
			LOG("Request: Error: No values found in header", 1);
			return false;
		}
		if (last_flag == false) {
			LOG("Request: Error: A comma found at the end of the header", 1);
			return false;
		}
		if (type == HEADER) {
			m_requestData.headers[key] = values;
		} else {
			m_requestData.trailingHeaders[key] = values;
		}
		return true;
	}

	bool Request::parseHeaders(std::string& input, HeaderType type) {
		std::string line;
		while (1) {
			GetLineStatus status = getNextLineHTTP(input, line);
			if (status == GET_LINE_END)
				break;
			if (status == GET_LINE_ERROR)
				return false;
			if (line == "") {
				if (type == HEADER) {
					m_status = PARSE_BODY;
				} else {
					m_status = PARSE_END;
				}
				return interpretHeaders(type);
			}
			if (!parseHeader(line, type)) {
				return false;
			}
			line.clear();
		}
		return true;
	}

	bool Request::interpretHeaders(HeaderType type) {
		(void)type;
		for (std::map<std::string, std::vector<std::string> >::iterator it = m_requestData.headers.begin(); it != m_requestData.headers.end(); ++it) {
			// TODO: Implement
			if (it->first == "Transfer-Encoding" && it->second[0] == "chunked") {
				if (m_requestData.headers["Content-Length"].size() > 0) {
					LOG("Request: Error: Both Content-Length and Transfer-Encoding header found", 1);
					return false;
				}
				m_expectedBody = CHUNKED;
			} else if (it->first == "Content-Length" && it->second.size() > 0) {
				if (m_expectedBody == CHUNKED) {
					LOG("Request: Error: Both Content-Length and Transfer-Encoding header found", 1);
					return false;
				}
				m_expectedBody = CONTENT_LENGTH;
				int i;
				std::istringstream(it->second[0]) >> i;
				m_contentLength = i;
			}
		}
		return true;
	}

	bool Request::checkHead(const std::vector<std::string>& args) {
		if (args[0] != "GET" && args[0] != "POST" && args[0] != "DELETE") {
			return false;
		}
		if (args[2] != "HTTP/1.1") {
			return false;
		}
		return true;
	}

	bool Request::parseHead(std::string& input) {
		std::string line;
		GetLineStatus status = getNextLineHTTP(input, line);
		if (status == GET_LINE_ERROR)
			return false;
		if (status == GET_LINE_END)
			return true;
		if (line == "") {
			LOG("Request: Error: Empty line in request line", 1);
			return false;
		}
		std::string key;
		std::vector<std::string> args;
		unsigned long pos = 0;
		for (int a = 0; a < 3; a++) {
			while (pos < line.size() && line[pos] != ' ') {
				key += line[pos];
				pos++;
			}
			pos++;
			args.push_back(key);
			key.clear();
		}
		if (args.size() == 3 && checkHead(args) == true) {
			m_requestData.method = args[0];
			m_requestData.uri = args[1];
			m_requestData.version = args[2];
			m_status = PARSE_HEADERS;
			return true;
		} else {
			LOG("Request: Error: Invalid request line", 1);
			return false;
		}
	}

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