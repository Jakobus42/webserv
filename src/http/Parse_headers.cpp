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
			// find values
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
				if (val.length() > 1000) {
					LOG("Request: Error: Too long value in header", 1);
					return false;
				}
				values.push_back(val);
				if (values.size() > 1000) {
					LOG("Request: Error: Too many values in header", 1);
					return false;
				}
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

} /* namespace http */