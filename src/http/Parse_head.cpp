#include "http/Request.hpp"

namespace http {

	bool checkHead(std::vector<std::string>& args) {
		if (args[0] != "GET" && args[0] != "POST" && args[0] != "PUT" && args[0] != "DELETE") {
			return false;
		}
		if (args[2] != "HTTP/1.1\r") {
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
					// ERROR MESSAGE
					return false;
				}
			}
			if (input[i] == '\n') {
				if (input[i - 1] != '\r') {
					// ERROR MESSAGE
					return false;
				}
				std::vector<std::string> args;
				std::stringstream stream(line);

				input = input.substr(i + 1);
				i = 0;
				while (getline(stream, key, ' ')) {
					args.push_back(key);
				}
				if (args.size() == 3 && checkHead(args) == true) {
					m_requestData.method = args[0];
					m_requestData.uri = args[1];
					m_requestData.version = args[2];
					m_status = PARSE_HEADERS;
					return true;
				} else {
					return false;
				}
			} else {
				line += input[i];
			}
		}
		return true;
	}

} /* namespace http */