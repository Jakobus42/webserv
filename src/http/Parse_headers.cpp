#include "http/Request.hpp"

namespace http {

bool Request::parseHeader(std::string &line)
{
	QuoteFlag qf = NO_QUOTE;
	std::string key = "";
	std::vector<std::string> values;
	int promptStart = 0;
	for (unsigned long i = 0; i < line.size(); i++)
	{
		//quote Management
		if ((line[i] == '\'' || line[i] == '\"') && qf == NO_QUOTE)
		{
			if (line[i] == '\'') {
				qf = SINGLE_QUOTE;
			}
			else {
				qf = DOUBLE_QUOTE;
			}
		}
		if ((line[i] == '\'' && qf == SINGLE_QUOTE) || (line[i] == '\"' && qf == DOUBLE_QUOTE))
		{
			qf = NO_QUOTE;
		}
		//find key
		if (line[i] == ':' && qf == NO_QUOTE)
		{
			if (i == 0)
			{
				//ERROR MESSAGE
				return false;
			}
			key = line.substr(0, i);
			promptStart = i;
		}
		//find values
		if ((line[i] == ',' && qf == NO_QUOTE) || i == line.size() - 1)
		{
			if (i == line.size() - 1)
			{
				i++;
			}
			if (key == "")
			{
				//ERROR MESSAGE
				return false;
			}
			std::string val = line.substr(promptStart, i - promptStart);
			val = val.substr(val.find_first_not_of(' ') + 1);
			if (val == "")
			{
				//ERROR MESSAGE
				return false;
			}
			values.push_back(val);
			promptStart = i;
		}
	}
	m_requestData.headers[key] = values;
	return true;
}

bool Request::parseHeaders(std::string &input) {
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
				input = input.substr(1);
				m_status = PARSE_BODY;
				if (!interpretHeaders())
				{
					return false;
				}
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

bool Request::interpretHeaders() 
{
	for (unsigned long i = 0; i < m_requestData.headers.size(); i++) 
	{
		//TODO: Implement
		if (m_requestData.headers["Transfer-Encoding"][0] == "chunked")
		{
			m_expectedBody = CHUNKED;
		}
		else if (m_requestData.headers["Content-Length"].size() > 0)
		{
			m_expectedBody = CONTENT_LENGTH;
			int i;
			std::istringstream(m_requestData.headers["Content-Length"][0]) >> i;
			m_contentLength = i;
			std::cout << "HI!!! Content-Length: " << m_contentLength << std::endl;
			
		}
	}
	return true;
}

} /* namespace http */