#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

struct location 
{
	//location name
	std::string name;
	//return
	std::string return_url;
	int return_code;
	//configurations
	std::vector<std::string> methods;
	bool autoindex;
	std::string root;
	std::vector<std::string> index;
	//locations
	std::vector<location> locations;
};

struct server 
{
	//configurations
	int port;
	std::vector<int> ip_address;
	std::vector<std::string> server_names;
	std::map<int, std::string> errorPages;
	unsigned long max_body_size;
	//locations
	std::vector<location> locations;
};


struct Config_data 
{
	//servers
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
		//ConfigFileParser(std::string &configFileName);
		int loadConfigFile(std::string &configFileName);
		void print_config_data(int detailed);

    private:
/* 		int m_port;
		std::string m_serverName; */
		Config_data m_configData;
		int is_loaded;
		
		std::string readConfigFile(std::string &configFileName);
		int parseConfigFile(std::string &configFileName, int layer, unsigned long &i, int &line_count);
		
		int handle_server(std::string &line, unsigned long *i);
		int handle_prompt(std::string &line, int layer, int &line_count);
		int SaveConfigData(std::vector<std::string> &args, int layer, int qoute_flag, int &line_count);
		int id_command(std::string &command);

		int server(std::vector<std::string> &args, int & line_count, int layer);
		int location(std::vector<std::string> &args, int & line_count, int layer);

		int listen(std::vector<std::string> &args, int & line_count, int layer);
 		int server_name(std::vector<std::string> &args, int & line_count, int layer);
		int error_page(std::vector<std::string> &args, int & line_count, int layer);
		int client_max_body_size(std::vector<std::string> &args, int & line_count, int layer);
 		int limit_except(std::vector<std::string> &args, int & line_count, int layer);
		int return_(std::vector<std::string> &args, int & line_count, int layer);
		int root(std::vector<std::string> &args, int & line_count, int layer);
		int autoindex(std::vector<std::string> &args, int & line_count, int layer);
		int index(std::vector<std::string> &args, int & line_count, int layer);

		void error_message(int & line_count, std::string message);
		void print_locations(std::vector<struct location> &locations, int layer, int detailed, std::vector<int> layer_num);
		struct location *get_location(int layer);
		
};

} /* namespace configfile */
