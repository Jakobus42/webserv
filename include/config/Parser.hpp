#pragma once

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include "config/Parser.hpp"
#include "logger/LoggerManager.hpp"

namespace configfile {

typedef struct s_location t_location;

typedef struct s_location {
  // location name
  std::string name;
  // return
  std::string return_url;
  // configurations
  std::vector<std::string> methods;
  bool autoindex;
  std::string root;
  std::vector<std::string> index;
  // locations
  std::vector<t_location> locations;
} t_location;

enum CmdId {
  SERVER_ID = 0,
  LISTEN_ID = 1,
  SERVER_NAME_ID = 2,
  ERROR_PAGE_ID = 3,
  CLIENT_MAX_BODY_SIZE_ID = 4,
  LOCATION_ID = 5,
  LIMIT_EXCEPT_ID = 6,
  RETURN_ID = 7,
  ROOT_ID = 8,
  AUTOINDEX_ID = 9,
  INDEX_ID = 10,
  UNKNOWN_ID = 404
};

typedef struct s_server {
  // configurations
  int port;
  uint32_t ip_address;
  std::vector<std::string> server_names;
  std::map<int, std::string> errorPages;
  unsigned long max_body_size;
  // locations
  std::vector<t_location> locations;
} t_server;

typedef struct s_config_data {
  // servers
  std::vector<t_server> servers;
} t_config_data;

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
  t_config_data getConfigData() const;
  int getServerSize() const;
  int getServerPort(int index) const;
  uint32_t getServerIp(int index) const;
  std::vector<std::string> getServerNames(int index) const;
  std::map<int, std::string> getErrorPages(int index) const;
  unsigned long getMaxBodySize(int index) const;
  t_server* getServer(int index);

  void printConfigData(int detailed);
  int testFunction(std::string const& key, std::vector<std::string>& args, int& lineCount);

 private:
  ConfigFileParser(const ConfigFileParser& other);
  ConfigFileParser& operator=(const ConfigFileParser& other);
  t_config_data m_configData;
  int m_isLoaded;

  int readConfigFile(std::string& configFileName, std::string& file);
  int parseConfigFile(std::string& configFileName, int layer, unsigned long& i, int& lineCount);

  int handleServer(std::string& line, unsigned long* i);
  int handlePrompt(std::string& line, int layer, int& lineCount);
  int SaveConfigData(std::vector<std::string>& args, int layer, int qoute_flag, int& lineCount);
  enum CmdId idCommand(std::string const& command);

  int server(std::vector<std::string>& args, int& lineCount, int layer);
  int location(std::vector<std::string>& args, int const& lineCount, int layer);

  int listen(std::vector<std::string>& args, int& lineCount, int layer);
  int serverName(std::vector<std::string>& args, int& lineCount, int layer);
  int errorPage(std::vector<std::string>& args, int& lineCount, int layer);
  int clientMaxBodySize(std::vector<std::string>& args, int& lineCount, int layer);
  int limitExcept(std::vector<std::string>& args, int& lineCount, int layer);
  int returnKeyword(std::vector<std::string>& args, int const& lineCount, int layer);
  int root(std::vector<std::string>& args, int& lineCount, int layer);
  int autoindex(std::vector<std::string>& args, int& lineCount, int layer);
  int index(std::vector<std::string>& args, int& lineCount, int layer);

  void printLocations(const std::vector<configfile::t_location>& locations, int layer, int detailed,
                      std::vector<int> layer_num);
  configfile::t_location* getLocation(int layer);
};

} /* namespace configfile */
