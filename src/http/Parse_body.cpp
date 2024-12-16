#include "http/Request.hpp"

namespace http {

	bool Request::parseBody(std::string& input) {
		std::cout << "---------------------------------" << std::endl;
		if (m_expectedBody == NO_BODY) {
			m_status = PARSE_END;
			return true;
		}
		if (m_expectedBody == CHUNKED) {
			if (!parseBodyChunked(input)) {
				return false;
			}
			return true;
		}
		if (m_expectedBody == CONTENT_LENGTH) {
			if (m_requestData.body.size() + input.size() < m_contentLength) {
				m_requestData.body += input;
				input = "";
				return true;
			} else {
				m_requestData.body += input.substr(0, m_contentLength - m_requestData.body.size());
				input = input.substr(m_contentLength - m_requestData.body.size());
				m_status = PARSE_END;
				return true;
			}
		}
		return true;
	}

	bool Request::parseBodyChunked(std::string& input) {
		std::string line;
		for (unsigned long i = 0; i < input.size(); i++) {
			if (m_contentLength != 0) {
				std::string chunk = input.substr(i, m_contentLength);
				m_contentLength -= chunk.size();
				m_requestData.body += chunk;
				input = input.substr(i + m_contentLength);
			} else if (input[i] == '\n') {
				if (line == "0") {
					m_status = PARSE_END;
					return true;
				}
				m_contentLength = shared::string::posStoi(line, 16);
				input = input.substr(i + 1);
				i = 0;
			} else {
				line += input[i];
			}
		}
		return true;
	}

} /* namespace http */
