#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "../../include/configfile/Parser.hpp"
#include "../../include/logger/LoggerManager.hpp"
struct location {
  // location name
  std::string name;
  // return
  std::string return_url;
  int return_code;
  // configurations
  std::vector<std::string> methods;
  bool autoindex;
  std::string root;
  std::vector<std::string> index;
  // locations
  std::vector<location> locations;
};

enum cmd_id {
  server_id = 0,
  listen_id = 1,
  server_name_id = 2,
  error_page_id = 3,
  client_max_body_size_id = 4,
  location_id = 5,
  limit_except_id = 6,
  return_id = 7,
  root_id = 8,
  autoindex_id = 9,
  index_id = 10
};

struct server {
  // configurations
  int port;
  std::vector<int> ip_address;
  std::vector<std::string> server_names;
  std::map<int, std::string> errorPages;
  unsigned long max_body_size;
  // locations
  std::vector<location> locations;
};

struct Config_data {
  // servers
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

  int loadConfigFile(std::string& configFileName);

  int getIsLoaded() const;
  Config_data getConfigData() const;

  void printConfigData(int detailed);
  int testFunction(std::string const& key, std::vector<std::string>& args, int& line_count);

 private:
  ConfigFileParser(const ConfigFileParser& other);
  ConfigFileParser& operator=(const ConfigFileParser& other);
  Config_data m_configData;
  int m_isLoaded;

  std::string readConfigFile(std::string& configFileName);
  int parseConfigFile(std::string& configFileName, int layer, unsigned long& i, int& line_count);

  int handleServer(std::string& line, unsigned long* i);
  int handlePrompt(std::string& line, int layer, int& line_count);
  int SaveConfigData(std::vector<std::string>& args, int layer, int qoute_flag, int& line_count);
  int idCommand(std::string const& command);

  int server(std::vector<std::string>& args, int& line_count, int layer);
  int location(std::vector<std::string>& args, int const& line_count, int layer);

  int listen(std::vector<std::string>& args, int& line_count, int layer);
  int serverName(std::vector<std::string>& args, int& line_count, int layer);
  int errorPage(std::vector<std::string>& args, int& line_count, int layer);
  int clientMaxBodySize(std::vector<std::string>& args, int& line_count, int layer);
  int limitExcept(std::vector<std::string>& args, int& line_count, int layer);
  int returnKeyword(std::vector<std::string>& args, int const& line_count, int layer);
  int root(std::vector<std::string>& args, int& line_count, int layer);
  int autoindex(std::vector<std::string>& args, int& line_count, int layer);
  int index(std::vector<std::string>& args, int& line_count, int layer);

  void printLocations(std::vector<struct location>& locations, int layer, int detailed, std::vector<int> layer_num);
  struct location* getLocation(int layer);
};

} /* namespace configfile */
