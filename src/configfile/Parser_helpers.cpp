#include "../../include/configfile/Parser.hpp"

namespace configfile {

bool is_number(const std::string& str)
{
	std::string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) 
		++it;
	if (it == str.end())
		return true;
	return false;
}

struct location * ConfigFileParser::get_location(int layer)
{
	struct location* temp = &m_configData.servers.back().locations.back();
	for (int i = 2; i < layer; i++)
	{
		temp = &temp->locations.back();
	}
	return temp;
}

int ft_stoi(std::string str)
{
	long res = 0;
	unsigned long i = 0;
	if (str.size() == 0)
	{
		return -1;
	}
	for (; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return -1;
		res = res * 10 + str[i] - '0';
		if (res > 2147483647)
			return -1;
	}

	return res;
}

int ConfigFileParser::server(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	if (args.size() != 1)
	{
		error_message(line_count, "Invalid number of arguments for server");
		return 1;
	}
	struct server new_server;
	//set default values
	new_server.port = 80;
	for (unsigned long i = 0; i < 4; i++)
		new_server.ip_address.push_back(0);
	new_server.max_body_size = 1000000;
	m_configData.servers.push_back(new_server);
	return 0;
}

int ConfigFileParser::location (std::vector<std::string> &args, int & line_count, int layer)
{
	if (args.size() != 2)
	{
		error_message(line_count, "Invalid number of arguments for location");
		return 1;
	}
	struct location new_location;
	//set default values
	new_location.autoindex = false;
	new_location.root = "";
	new_location.name = args[1];
	if (layer == 1)
	{
		m_configData.servers.back().locations.push_back(new_location);
		return 0;
	}
	struct location* temp = &m_configData.servers.back().locations.back();
	for (int i = 2; i < layer; i++)
	{
		temp = &temp->locations.back();
	}
	temp->locations.push_back(new_location);
	return 0;
}

int ConfigFileParser::listen (std::vector<std::string> &args, int & line_count, int layer)
{	
	(void)layer;
	if (args.size() != 2)
	{
		error_message(line_count, "Invalid number of arguments for listen");
		return 1;
	}
	if (is_number(args[1]))
	{
		int i = ft_stoi(args[1]);
		if (i < 0 || i > 65535)
		{
			error_message(line_count, "Invalid number for listen");
			return 1;
		}
		m_configData.servers.back().port = i;
	}
	else
	{
		for (unsigned long i = 0; i < 4; i++)
		{
			std::size_t iter;
			if (i == 3)
			{
				iter = args[1].find(':');
				if (iter != std::string::npos)
				{
					int i = ft_stoi(args[1].substr(iter + 1));
					if (i < 0 || i > 65535)
					{
						error_message(line_count, "Invalid number for listen");
						return 1;
					}
					m_configData.servers.back().port = i;
				}
			}
			else
			{
				iter = args[1].find('.');
				if (iter == std::string::npos)
				{
					error_message(line_count, "Invalid number for listen");
					return 1;
				}
			}
			std::string temp = args[1].substr(0, iter);
			int j = ft_stoi(temp);
			if (j < 0 || j > 255)
			{
				error_message(line_count, "Invalid number for listen");
				return 1;
			}
			args[1] = args[1].substr(iter + 1);
			m_configData.servers.back().ip_address[i] = j;
		}
		if (args[1].size() != 0)
		{
			int i = ft_stoi(args[1]);
			if (i < 0 || i > 65535)
			{
				error_message(line_count, "Invalid number for listen");
				return 1;
			}
			m_configData.servers.back().port = i;
		}
	}
	return 0;
}

int ConfigFileParser::server_name(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	for (unsigned long i = 1; i < args.size(); i++)
	{
		m_configData.servers.back().server_names.push_back(args[i]);
		if (i > 1000)
		{
			error_message(line_count, "Too many server names");
			return 1;
		} 
	}
	return 0;
}

int ConfigFileParser::error_page(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	std::string last = args.back();
	//TODO: check if last is a valid path?
	args.pop_back();
	for (unsigned long i = 1; i < args.size(); i++)
	{
		int j = ft_stoi(args[i]);
		if (j < 100 || j > 599)
		{
			error_message(line_count, "Invalid number for error_page");
			return 1;
		}
		m_configData.servers.back().errorPages[j] = last;
	}
	return 0;
}

int ConfigFileParser::client_max_body_size(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	if (args.size() != 2)
	{
		error_message(line_count, "Invalid number of arguments for client_max_body_size");
		return 1;
	}
	int i = ft_stoi(args[1]);
	if (i < 0)
	{
		error_message(line_count, "Invalid number for client_max_body_size");
		return 1;
	}
	m_configData.servers.back().max_body_size = i;
	return 0;
}

int ConfigFileParser::limit_except(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("HEAD");
	methods.push_back("POST");
	methods.push_back("PUT");
	methods.push_back("DELETE");
	methods.push_back("MKCOL");
	methods.push_back("COPY");
	methods.push_back("MOVE");
	methods.push_back("OPTIONS");
	methods.push_back("PROPFIND");
	methods.push_back("PROPPATCH");
	methods.push_back("LOCK");
	methods.push_back("UNLOCK");
	methods.push_back("PATCH");
	std::vector<std::string> allowed_methods;
	for (unsigned long i = 1; i < args.size(); i++)
	{
		for (unsigned long j = 0; j < methods.size(); j++)
		{
			if (args[i] == methods[j])
			{
				if (args[i] == "GET")
				{
					allowed_methods.push_back(args[i]);
					methods.erase(methods.begin() + j);
					allowed_methods.push_back("HEAD");
					break;
				}
				else
				{
					allowed_methods.push_back(args[i]);
					methods.erase(methods.begin() + j);
					break;
				}
			}
			if (j == methods.size() - 1)
			{
				error_message(line_count, "Invalid method for limit_except");
				return 1;
			}
		}
	}
	if (allowed_methods.size() == 0)
	{
		error_message(line_count, "No methods found for limit_except");
		return 1;
	}
	struct location* current = get_location(layer);
	current->methods = allowed_methods;
	std::cout << "location: " << current->name << std::endl;
	return 0;
}

/* int ConfigFileParser::return_(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	return 0;
} */

int ConfigFileParser::root(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	if (args.size() != 2)
	{
		error_message(line_count, "Invalid number of arguments for root");
		return 1;
	}
	//TODO: check if path is valid
	struct location* current = get_location(layer);
	current->root = args[1];
	return 0;
}

int ConfigFileParser::autoindex(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	if (args.size() != 2)
	{
		error_message(line_count, "Invalid number of arguments for autoindex");
		return 1;
	}
	struct location* current = get_location(layer);
	if (args[1] == "on")
	{
		current->autoindex = true;
	}
	else if (args[1] == "off")
	{
		current->autoindex = false;
	}
	else
	{
		error_message(line_count, "Invalid argument for autoindex");
		return 1;
	}
	return 0;
}

/* int ConfigFileParser::index(std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	struct location* current = get_location(layer);
	for (unsigned long i = 1; i < args.size(); i++)
	{
		current->index.push_back(args[i]);
	}
	return 0;
} */

void ConfigFileParser::print_config_data(int detailed)
{
	for (unsigned long i = 0; i < m_configData.servers.size(); i++)
	{
		std::cout << "--------------------------" << std::endl;
		std::cout << "Server: " << i << std::endl;
		if (detailed)
		{
			std::cout << "Port: " << m_configData.servers[i].port << std::endl;
			std::cout << "IP address: ";
			for (unsigned long j = 0; j < m_configData.servers[i].ip_address.size(); j++)
			{
				std::cout << m_configData.servers[i].ip_address[j] << ".";
			}
			std::cout << std::endl;
			std::cout << "Server names: ";
			for (unsigned long j = 0; j < m_configData.servers[i].server_names.size(); j++)
			{
				std::cout << m_configData.servers[i].server_names[j] << " ";
			}
			std::cout << std::endl;
			std::cout << "Error pages: ";
			for (std::map<int, std::string>::iterator it = m_configData.servers[i].errorPages.begin(); it != m_configData.servers[i].errorPages.end(); it++)
			{
				std::cout << it->first << " " << it->second << " ";
			}
			std::cout << std::endl;
			std::cout << "Max body size: " << m_configData.servers[i].max_body_size << std::endl;
			std::cout << "Locations: " << std::endl;
		}
		print_locations(m_configData.servers[i].locations, 1, detailed);
	}
}

void ConfigFileParser::print_locations(std::vector<struct location> &locations, int layer, int detailed)
{
	std::string c = "";
	for (int i = 0; i < layer; i++)
	{
		c += "  ";
		c += "|";
	}
	for (unsigned long j = 0; j < locations.size(); j++)
	{
		std::cout << "--------------------------" << std::endl;
		std::cout << c << "Location: " << j << " name: " << locations[j].name << std::endl;
		if (detailed)
		{
			std::cout << c<< "Root: " << locations[j].root << std::endl;
			std::cout << c << "Autoindex: " << locations[j].autoindex << std::endl;
			std::cout << c << "Methods: "; 
			for (unsigned long k = 0; k < locations[j].methods.size(); k++)
			{
				std::cout << locations[j].methods[k] << " ";
			}
			std::cout << std::endl;
			std::cout << c << "Locations: " << std::endl;
		}
		//go one step deeper
		if (locations[j].locations.size() > 0)
			print_locations(locations[j].locations, layer + 1, detailed);
	}
}

} /* namespace configfile */