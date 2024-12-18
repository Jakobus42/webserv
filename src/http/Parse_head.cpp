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

	bool Request::parseHead(std::string& input) {
		std::string line;
		std::string key;
		for (unsigned long i = 0; i < input.size(); i++) {
			if (input[i] == '\r') {
				if (i != input.length() - 1 && input[i + 1] != '\n') {
					LOG("Request: Error: Invalid line ending in request line", 1);
					return false;
				}
			}
			if (input[i] == '\n') {
				if (input[i - 1] != '\r') {
					LOG("Request: Error: Invalid line ending in  request line", 1);
					return false;
				}
				line = line.substr(0, line.size() - 1);
				if (line == "") {
					LOG("Request: Error: Empty line in  request line", 1);
					return false;
				}
				input = input.substr(i + 1);
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
			} else {
				line += input[i];
			}
		}
		return true;
	}

} /* namespace http */