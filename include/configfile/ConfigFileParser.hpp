#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

struct location {
	//location name
	std::string name;
	//flags

	//http methods
	std::vector<std::string> methods;
	//locations
};

struct server 
{
	//configurations
	int port;
	std::string host;
	std::vector<std::string> server_names;
	std::map<int, std::string> errorPages;
	unsigned long max_body_size;

	//http methods
	std::vector<std::string> methods;
	//locations
	std::vector<location> locations;
};


struct Config_data {
	//flags
	//locations
	std::vector<server> servers;
};

namespace configfile {

/**
 * @class ConfigFileParser
 * @brief This class is responsible for parsing the configuration file
 */
class ConfigFileParser {
    public:
        ConfigFileParser();
        ~ConfigFileParser();
        ConfigFileParser(const ConfigFileParser &other);
        ConfigFileParser& operator=(const ConfigFileParser &other);
		ConfigFileParser(std::string &configFileName);
		int loadConfigFile(std::string &configFileName);

    private:
/* 		int m_port;
		std::string m_serverName; */
		Config_data m_configData;
		std::string readConfigFile(std::string &configFileName);
		int parseConfigFile(std::string &configFileName, int layer, unsigned long &i);
		int handle_server(std::string &line, unsigned long *i);
		int handle_prompt(std::string &line, int layer);
};

} /* namespace configfile */
