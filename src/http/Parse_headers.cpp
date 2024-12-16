#include "http/Request.hpp"

namespace http {

	bool Request::parseHeader(std::string& line) {
		QuoteFlag qf = NO_QUOTE;
		std::string key = "";
		std::vector<std::string> values;
		int promptStart = 0;
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
			if (line[i] == ':' && qf == NO_QUOTE) {
				if (i == 0) {
					// ERROR MESSAGE
					return false;
				}
				key = line.substr(0, i);
				promptStart = i;
			}
			// find values
			if ((line[i] == ',' && qf == NO_QUOTE) || i == line.size() - 1) {
				if (i == line.size() - 1) {
					i++;
				}
				if (key == "") {
					// ERROR MESSAGE
					return false;
				}
				std::string val = line.substr(promptStart, i - promptStart);
				val = val.substr(val.find_first_not_of(' ') + 1);
				if (val == "") {
					// ERROR MESSAGE
					return false;
				}
				values.push_back(val);
				promptStart = i;
			}
		}
		m_requestData.headers[key] = values;
		return true;
	}

	bool Request::parseHeaders(std::string& input) {
		std::string line;

		for (unsigned long i = 0; i < input.size(); i++) {
			if (input[i] == '\r') {
				if (i != input.length() - 1 && input[i + 1] != '\n') {
					// ERROR MESSAGE
					std::cout << "cde" << std::endl;
					return false;
				}
			} else if (input[i] == '\n') {
				if (i != 0 && input[i - 1] != '\r') {
					// ERROR MESSAGE
					return false;
				}
				if (!parseHeader(line)) {
					// ERROR MESSAGE
					return false;
				}
				line.clear();
				if (input[i + 1] == '\r' && input[i + 2] == '\n') {
					input = input.substr(i + 3);
					m_status = PARSE_BODY;
					if (!interpretHeaders()) {
						// ERROR MESSAGE
						return false;
					}
					return true;
				}
				input = input.substr(i);
				i = 0;
			} else {
				line += input[i];
			}
		}
		return true;
	}

	/* bool Request::parseHeaders(std::string &input)
	{
		std::string line;

		for (unsigned long i = 0; i < input.size(); i++)
		{
			if (input[i] == '\n')
			{
				if (!parseHeader(line))
				{
					return false;
				}
				line.clear();
				input = input.substr(i);
				i = 0;
				if (input[i + 1] == '\n')
				{
					input = input.substr(2);
					m_status = PARSE_BODY;
					if (!interpretHeaders())
					{
						return false;
					}
					std::cout << "found end of headers" << std::endl;
					return true;
				}
			}
			else
			{
				line += input[i];
			}
		}
		return true;
	}
	 */
	bool Request::interpretHeaders() {
		/* 	if (m_requestData.headers.empty())
			{
				//ERROR MESSAGE
				return false;
			} */
		for (std::map<std::string, std::vector<std::string> >::iterator it = m_requestData.headers.begin(); it != m_requestData.headers.end(); ++it) {
			// TODO: Implement
			if (it->first == "Transfer-Encoding" && it->second[0] == "chunked") {
				if (m_requestData.headers["Content-Length"].size() > 0) {
					// ERROR MESSAGE
					return false;
				}
				m_expectedBody = CHUNKED;
			} else if (it->first == "Content-Length" && it->second.size() > 0) {
				if (m_expectedBody == CHUNKED) {
					// ERROR MESSAGE
					return false;
				}
				m_expectedBody = CONTENT_LENGTH;
				int i;
				std::istringstream(it->second[0]) >> i;
				m_contentLength = i;
			}
			/* 		else if ((it->first == "Connection") && (it->second[0] == "close" || "keep-alive") )
					{

					} */
		}
		return true;
	}

} /* namespace http */