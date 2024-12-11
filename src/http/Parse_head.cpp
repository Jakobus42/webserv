#include "http/Request.hpp"

bool checkHead(std::vector<std::string> &args) {
	if (args[0] != "GET" && args[0] != "POST" && args[0] != "PUT" && args[0] != "DELETE") 
	{
		return false;
	}
	if (args[2] != "HTTP/1.1") 
	{
		return false;
	}
	return true;
}

bool http::Request::parseHead(std::string &input) {
	std::string line;
	std::string key;
	for (int i = 0; i < input.size(); i++) {
		if (input[i] == '\n') 
		{
			std::vector<std::string> args;
			std::stringstream stream(line);
			
			input = input.substr(i + 1);
			while (getline(stream, key, ' ')) 
			{
				args.push_back(key);
			}
			if (args.size() == 3 && checkHead(args) == true) 
			{
				m_requestData.method = args[0];
				m_requestData.uri = args[1];
				m_requestData.version = args[2];
				m_status = PARSE_HEADERS;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			line += input[i];
		}
	}
	return true;
}