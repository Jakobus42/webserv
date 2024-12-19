#include "http/Request.hpp"

namespace http {

	bool Request::checkHead(const std::vector<std::string>& args) {
		if (args[0] != "GET" && args[0] != "POST" && args[0] != "PUT" && args[0] != "DELETE") {
			return false;
		}
		if (args[2] != "HTTP/1.1") {
			return false;
		}
		return true;
	}


	bool Request::parseHead(std::string& input)
	{
		std::string line;
		GetLineStatus status =  getNextLineHTTP(input, line);
		if (status == GET_LINE_ERROR)
			return false;
		if (status == GET_LINE_END)
			return true;
		if (line == "") {
			LOG("Request: Error: Empty line in  request line", 1);
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

} /* namespace http */