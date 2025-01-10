#include "http/Request.hpp"

namespace http {

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
			std::cout << "Request: Error: Empty line in request line" << std::endl;
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
			std::cout << "Request: Error: Invalid request line" << std::endl;
			return false;
		}
	}

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
					std::cout << "Request: Error: Empty key in header" << std::endl;
					return false;
				}
				if (shared::string::isGreyspace(line[i - 1])) {
					std::cout << "Request: Error: Found whitespace before colon in the header section" << std::endl;
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
					std::cout << "Request: Error: Found value before key" << std::endl;
					return false;
				}
				if (i == promptStart) {
					std::cout << "Request: Error: Empty value in header" << std::endl;
					return false;
				}
				if (shared::string::isGreyspace(line[i - 1])) {
					std::cout << "Request: Error: Found whitespace before comma in the header section" << std::endl;
					return false;
				}
				while (shared::string::isGreyspace(line[promptStart])) {
					promptStart++;
				}
				std::string val = line.substr(promptStart, i - promptStart);
				if (val.length() > 1000) {
					std::cout << "Request: Error: value in header too long " << std::endl;
					return false;
				}
				values.push_back(val);
				if (values.size() > 1000) {
					std::cout << "Request: Error: Too many values in header" << std::endl;
					return false;
				}
				promptStart = i + 1;
			}
		}
		if (key == "") {
			std::cout << "Request: Error: No key found in header" << std::endl;
			return false;
		}
		if (values.size() == 0) {
			std::cout << "Request: Error: No values found in header" << std::endl;
			return false;
		}
		if (last_flag == false) {
			std::cout << "Request: Error: A comma found at the end of the header" << std::endl;
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
					std::cout << "Request: Error: Both Content-Length and Transfer-Encoding header found" << std::endl;
					return false;
				}
				m_expectedBody = CHUNKED;
			} else if (it->first == "Content-Length" && it->second.size() > 0) {
				if (m_expectedBody == CHUNKED) {
					std::cout << "Request: Error: Both Content-Length and Transfer-Encoding header found" << std::endl;
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
				if (!parseBodyChunkedSize(input)) {
					return false;
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
					std::cout << "Error: Invalid chunked encoding, \r\n expected after chunk." << std::endl;
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
				std::cout << "Error: Empty line in chunked encoding" << std::endl;
				return false;
			} else {
				int ret = 0;
				m_contentLength = shared::string::StoiHex(line, ret);
				if (ret == -1) {
					std::cout << "Error: Invalid hexadecimal number in chunked encoding" << std::endl;
					return false;
				}
				if (m_contentLength == 0) {
					m_chunkedStatus = CHUNK_END;
				} else {
					m_chunkedStatus = CHUNK_DATA;
				}
				if (line != "") {
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
				std::cout << "Error: Invalid chunked extention, \";\" expected after chunk size or other chunked extention" << std::endl;
				//LOG("Error: Invalid chunked extention, \";\" expected after chunk size or other chunked extention", 1);
				return false;
			}
			removeWhitespaces(line);
			while (line != "" && line[0] != ' ' && line[0] != '\t' && line[0] != '=' && line[0] != ';') { // find Key
				key += line[0];
				if (key.length() > 100) {
					//LOG("Error: Invalid chunked extention, key too long", 1);
					std::cout << "Error: Invalid chunked extention, key too long" << std::endl;
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
						//LOG("Error: Invalid chunked extention, value too long", 1);
						std::cout << "Error: Invalid chunked extention, value too long" << std::endl;
						return false;
					}
					line = line.substr(1);
				}
			}
			if (key == "") {
				//LOG("Error: Invalid chunked extention, (non-empty) key expected", 1);
				std::cout << "Error: Invalid chunked extention, (non-empty) key expected" << std::endl;
				return false;
			}
			if (hasValue && value == "") {
				//LOG("Error: Invalid chunked extention, (non-empty) value expected", 1);
				std::cout << "Error: Invalid chunked extention, (non-empty) value expected" << std::endl;
				return false;
			}
			chunkedExtension.extensions[key] = value; // save that chunked extention data
			if (chunkedExtension.extensions.size() > 1000) {
				//LOG("Error: Invalid chunked extention, too many extensions", 1);
				std::cout << "Error: Invalid chunked extention, too many extensions" << std::endl;
				return false;
			}
		}
		m_requestData.chunkedExtensions.push_back(chunkedExtension);
		return true;
	}

} /* namespace http */