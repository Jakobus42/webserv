#include "http/Request.hpp"

bool http::Request::parseHeaders(std::string &input) {
	std::string line;
	for (int i; i < input.size(); i++) 
	{
		if (input[i] == '\n') 
		{
			input = input.substr(i + 1);
			if (input[i + 1] == '\n')
			{
				input = input.substr(1);
				m_status = PARSE_BODY;
				return true;
			}
			if (line[0] != ' ')
			{
				if (line.find(':') == std::string::npos)
				{
					return false;
				}
				std::string key;
				std::string value;
				std::stringstream stream(line);
				getline(stream, key, ':');
				if (key == "")
				{
					return false;
				}
				while (getline(stream, value, ','))
				{
					m_requestData.headers[key].push_back(value);
				}
			}
			else
			{
				if (line.find(':') == std::string::npos)
				{
					return false;
				}

			}
			line.clear();
			i = 0;
		}
		else
		{
			line += input[i];
		}
	}
	return true;
}