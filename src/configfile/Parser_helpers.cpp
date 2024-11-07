#include "../../include/configfile/Parser.hpp"

namespace configfile {

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
	new_server.host = "localhost";
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
	//for every layer above 2, the location is in another location
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
	//m_configData.servers.back().locations.push_back(new_location);
	return 0;
}

int ConfigFileParser::listen (std::vector<std::string> &args, int & line_count, int layer)
{
	(void)layer;
	(void)line_count;
	(void)args;
	return 0;
}

void ConfigFileParser::print_config_data(int detailed)
{
	for (unsigned long i = 0; i < m_configData.servers.size(); i++)
	{
		std::cout << "--------------------------" << std::endl;
		std::cout << "Server: " << i << std::endl;
		if (detailed)
		{
			std::cout << "Port: " << m_configData.servers[i].port << std::endl;
			std::cout << "Host: " << m_configData.servers[i].host << std::endl;
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
			std::cout << c << "Locations: " << std::endl;
		}
		//go one step deeper
		if (locations[j].locations.size() > 0)
			print_locations(locations[j].locations, layer + 1, detailed);
	}
}





} /* namespace configfile */