#include "http/Request.hpp"

namespace http {

	bool Request::parseHeader(std::string& line, headerType type) {
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
			m_requestData.trailing_headers[key] = values;
		}
		return true;
	}

	bool Request::parseHeaders(std::string& input, headerType type) {
		std::string line;
		for (unsigned long i = 0; i < input.size(); i++) {
			if (input[i] == '\r') {
				if (i != input.length() - 1 && input[i + 1] != '\n') {
					LOG("Request: Error: Invalid line ending in header", 1);
					return false;
				}
			} else if (input[i] == '\n') {
				if (i != 0 && input[i - 1] != '\r') {
					LOG("Request: Error: Invalid line ending in header", 1);
					return false;
				}
				if (line == "") {
					input = input.substr(i + 1);
					if (type == HEADER) {
						m_status = PARSE_BODY;
					} else {
						m_status = PARSE_END;
					}
					if (!interpretHeaders(type)) {
						return false;
					}
					return true;
				}
				if (!parseHeader(line, type)) {
					return false;
				}
				line.clear();
				input = input.substr(i + 1);
				i = -1;
			} else {
				line += input[i];
			}
		}
		return true;
	}

	bool Request::interpretHeaders(headerType type) {
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

} /* namespace http */