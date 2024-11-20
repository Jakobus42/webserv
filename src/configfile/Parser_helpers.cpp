#include "../../include/configfile/Parser.hpp"

namespace configfile {
/**
 * @brief Checks if a string is a number.
 * @param str The string to check.
 * @return true if the string is a number, false otherwise.
 */
bool is_number(const std::string& str) 
{
  std::string::const_iterator it = str.begin();
  while (it != str.end() && std::isdigit(*it)) ++it;
  if (it == str.end()) return true;
  return false;
}

/**
 * @brief finds the current location in the configuration file.
 *
 * @param layer the current layer of the configuration file.
 * @return struct location* the current location.
 */
struct location* ConfigFileParser::getLocation(int layer) {
  struct location* temp = &m_configData.servers.back().locations.back();
  for (int i = 2; i < layer; i++) {
    temp = &temp->locations.back();
  }
  return temp;
}

/**
 * @brief Converts a string to a positive integer.
 * @param str The string to convert.
 * @return int The integer value of the string. or -1 if the string is not a number.
 */
int ft_stoi(std::string str) {
  long res = 0;
  unsigned long i = 0;
  if (str.size() == 0) {
    return -1;
  }
  for (; i < str.size(); i++) {
    if (!std::isdigit(str[i])) return -1;
    res = res * 10 + str[i] - '0';
    if (res > 2147483647) return -1;
  }

  return res;
}

/**
 * @brief Creates a new server struct, initializes it and adds it to the config data.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::server(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() != 1) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for server" << std::endl,
        logger::ERROR);
    return 1;
  }
  struct server new_server;
  // set default values
  new_server.port = 80;
  for (unsigned long i = 0; i < 4; i++) new_server.ip_address.push_back(0);
  new_server.max_body_size = 1000000;
  m_configData.servers.push_back(new_server);
  // TODO: Load default Error pages if none are provided
  return 0;
}

/**
 * @brief Creates a new location struct, initializes it and adds it to the config data.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::location(std::vector<std::string>& args, int& line_count, int layer) {
  if (args.size() != 2) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for location" << std::endl,
        logger::ERROR);
    return 1;
  }
  if (args[1].length() > 1000 || args[1].length() == 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Location name length invalid" << std::endl,
        logger::ERROR);
    return 1;
  }
  struct location new_location;
  // set default values
  new_location.autoindex = false;
  new_location.root = "";
  new_location.name = args[1];
  new_location.return_code = 0;
  new_location.return_url = "";
  new_location.index.push_back("index.html");
  if (layer == 1) {
    m_configData.servers.back().locations.push_back(new_location);
    return 0;
  }
  struct location* temp = &m_configData.servers.back().locations.back();
  for (int i = 2; i < layer; i++) {
    temp = &temp->locations.back();
  }
  temp->locations.push_back(new_location);
  return 0;
}

/**
 * @brief Sets the port and ip address of the server.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::listen(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() != 2) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for listen" << std::endl,
        logger::ERROR);
    return 1;
  }
  if (is_number(args[1])) {
    int i = ft_stoi(args[1]);
    if (i < 0 || i > 65535) {
      LOG("Configuration file (line " << line_count << "): "
                                      << "Invalid number for listen" << std::endl,
          logger::ERROR);
      return 1;
    }
    m_configData.servers.back().port = i;
  } else {
    for (unsigned long i = 0; i < 4; i++) {
      std::size_t iter;
      if (i == 3) {
        iter = args[1].find(':');
        if (iter != std::string::npos) {
          int i = ft_stoi(args[1].substr(iter + 1));
          if (i < 0 || i > 65535) {
            LOG("Configuration file (line " << line_count << "): "
                                            << "Invalid number for listen" << std::endl,
                logger::ERROR);
            return 1;
          }
          m_configData.servers.back().port = i;
        }
      } else {
        iter = args[1].find('.');
        if (iter == std::string::npos) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Invalid number for listen" << std::endl,
              logger::ERROR);
          return 1;
        }
      }
      std::string temp = args[1].substr(0, iter);
      int j = ft_stoi(temp);
      if (j < 0 || j > 255) {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Invalid number for listen" << std::endl,
            logger::ERROR);
        return 1;
      }
      args[1] = args[1].substr(iter + 1);
      m_configData.servers.back().ip_address[i] = j;
    }
    if (args[1].size() != 0) {
      int i = ft_stoi(args[1]);
      if (i < 0 || i > 65535) {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Invalid number for listen" << std::endl,
            logger::ERROR);
        return 1;
      }
      m_configData.servers.back().port = i;
    }
  }
  return 0;
}

/**
 * @brief Sets the server name(s) of the server.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::serverName(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() < 2) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for server_name" << std::endl,
        logger::ERROR);
    return 1;
  }
  for (unsigned long i = 1; i < args.size(); i++) {
    if (args[i].length() > 1000 || args[i].length() == 0) {
      LOG("Configuration file (line " << line_count << "): "
                                      << "Server name length invalid" << std::endl,
          logger::ERROR);
      return 1;
    }
    m_configData.servers.back().server_names.push_back(args[i]);
    if (i > 1000) {
      LOG("Configuration file (line " << line_count << "): "
                                      << "Too many server names" << std::endl,
          logger::ERROR);
      return 1;
    }
  }
  return 0;
}

/**
 * @brief sets the error pages for the server.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::errorPage(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() < 3 || args.size() > 1001) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for error_page" << std::endl,
        logger::ERROR);
    return 1;
  }
  std::string last = args.back();
  if (last.length() > 1000 || last.length() == 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Error page length invalid" << std::endl,
        logger::ERROR);
    return 1;
  }
  // TODO: check if last is a valid path?
  args.pop_back();
  for (unsigned long i = 1; i < args.size(); i++) {
    int j = ft_stoi(args[i]);
    if (j < 100 || j > 599) {
      LOG("Configuration file (line " << line_count << "): "
                                      << "Invalid number for error_page" << std::endl,
          logger::ERROR);
      return 1;
    }
    m_configData.servers.back().errorPages[j] = last;
  }
  return 0;
}

/**
 * @brief sets the max body size for the server.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::clientMaxBodySize(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() != 2) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for client_max_body_size" << std::endl,
        logger::ERROR);
    return 1;
  }
  int i = ft_stoi(args[1]);
  if (i < 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number for client_max_body_size" << std::endl,
        logger::ERROR);
    return 1;
  }
  m_configData.servers.back().max_body_size = i;
  return 0;
}

/**
 * @brief sets the methods allowed for the location.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::limitExcept(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() < 2 || args.size() > 15) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for limit_except" << std::endl,
        logger::ERROR);
    return 1;
  }
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
  for (unsigned long i = 1; i < args.size(); i++) {
    for (unsigned long j = 0; j < methods.size(); j++) {
      if (args[i] == methods[j]) {
        if (args[i] == "GET") {
          allowed_methods.push_back(args[i]);
          methods.erase(methods.begin() + j);
          allowed_methods.push_back("HEAD");
          break;
        } else {
          allowed_methods.push_back(args[i]);
          methods.erase(methods.begin() + j);
          break;
        }
      }
      if (j == methods.size() - 1) {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Invalid method for limit_except" << std::endl,
            logger::ERROR);
        return 1;
      }
    }
  }
  if (allowed_methods.size() == 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "No methods found for limit_except" << std::endl,
        logger::ERROR);
    return 1;
  }
  struct location* current = getLocation(layer);
  current->methods = allowed_methods;
  return 0;
}

/**
 * @brief redirects the user to a different url with a specific return code.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::returnKeyword(std::vector<std::string>& args, int& line_count, int layer) {
  if (args.size() != 3) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for return" << std::endl,
        logger::ERROR);
    return 1;
  }
  int i = ft_stoi(args[1]);
  if (i < 300 || i > 399) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid first argument for return, expected number between 300 and 399"
                                    << std::endl,
        logger::ERROR);
    return 1;
  }
  if (args[2].length() > 1000 || args[2].length() == 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Return url length invalid" << std::endl,
        logger::ERROR);
    return 1;
  }
  // TODO: check if valid url?
  struct location* current = getLocation(layer);
  current->return_code = i;
  current->return_url = args[2];
  return 0;
}

/**
 * @brief sets the root directory for the location.
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::root(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() != 2) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for root" << std::endl,
        logger::ERROR);
    return 1;
  }
  if (args[1].length() > 1000 || args[1].length() == 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Root length invalid" << std::endl,
        logger::ERROR);
    return 1;
  }
  // TODO: check if path is valid
  struct location* current = getLocation(layer);
  current->root = args[1];
  return 0;
}

/**
 * @brief turns the autoindex on or off for the location. (off by default)
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::autoindex(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;
  if (args.size() != 2) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for autoindex" << std::endl,
        logger::ERROR);
    return 1;
  }
  struct location* current = getLocation(layer);
  if (args[1] == "on") {
    current->autoindex = true;
  } else if (args[1] == "off") {
    current->autoindex = false;
  } else {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid argument for autoindex" << std::endl,
        logger::ERROR);
    return 1;
  }
  return 0;
}

/**
 * @brief sets the index files for the location. (index.html by default)
 * @param args all prompts in the current line of the configuration file.
 * @param line_count the current line number, so we are able to print error messages.
 * @param layer the current layer of the configuration file.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::index(std::vector<std::string>& args, int& line_count, int layer) {
  (void)layer;

  if (args.size() < 2 || args.size() > 1001) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Invalid number of arguments for index" << std::endl,
        logger::ERROR);
    return 1;
  }
  struct location* current = getLocation(layer);
  current->index.clear();
  for (unsigned long i = 1; i < args.size(); i++) {
    if (args[i].length() > 1000 || args[i].length() == 0) {
      LOG("Configuration file (line " << line_count << "): "
                                      << "Index file length invalid" << std::endl,
          logger::ERROR);
      return 1;
    }
    current->index.push_back(args[i]);
  }
  return 0;
}

/**
 * @brief prints the configuration data.
 * @param detailed toggles between detailed and simple output. (0 for simple, 1 for detailed)
 */
void ConfigFileParser::printConfigData(int detailed) {
  if (m_isLoaded == 0) {
    std::cout << "No configuration file loaded" << std::endl;
    return;
  }
  for (unsigned long i = 0; i < m_configData.servers.size(); i++) {
    std::cout << "--------------------------" << std::endl;
    std::cout << "Server: " << i + 1 << std::endl;
    if (detailed) {
      std::cout << "Port: " << m_configData.servers[i].port << std::endl;
      std::cout << "IP address: ";
      for (unsigned long j = 0; j < m_configData.servers[i].ip_address.size(); j++) {
        std::cout << m_configData.servers[i].ip_address[j];
        if (j != m_configData.servers[i].ip_address.size() - 1) std::cout << ".";
      }
      std::cout << std::endl;
      std::cout << "Server names: ";
      for (unsigned long j = 0; j < m_configData.servers[i].server_names.size(); j++) {
        std::cout << m_configData.servers[i].server_names[j] << " ";
      }
      std::cout << std::endl;
      std::cout << "Error pages: ";
      for (std::map<int, std::string>::iterator it = m_configData.servers[i].errorPages.begin();
           it != m_configData.servers[i].errorPages.end(); it++) {
        std::cout << it->first << " " << it->second << " ";
      }
      std::cout << std::endl;
      std::cout << "Max body size: " << m_configData.servers[i].max_body_size << std::endl;
      std::cout << "Locations: " << std::endl;
    }
    std::vector<int> layer_num;
    printLocations(m_configData.servers[i].locations, 1, detailed, layer_num);
  }
}

/**
 * @brief prints the locations of the server.
 * @param locations the locations to print.
 * @param layer the current layer of the configuration file.
 * @param detailed toggles between detailed and simple output. (0 for simple, 1 for detailed)
 * @param layer_num the current layer number.
 */
void ConfigFileParser::printLocations(std::vector<struct location>& locations, int layer, int detailed,
                                      std::vector<int> layer_num) {
  std::string c = "";
  for (int i = 0; i < layer; i++) {
    c += "   ";
    c += "|";
  }
  layer_num.push_back(1);
  for (unsigned long j = 0; j < locations.size(); j++) {
    std::cout << "--------------------------" << std::endl;
    std::cout << c << "Location: ";
    for (unsigned long k = 0; k < layer_num.size(); k++) {
      std::cout << layer_num[k];
      if (k != layer_num.size() - 1) std::cout << ".";
    }
    std::cout << " name: " << locations[j].name << std::endl;
    if (detailed) {
      if (locations[j].root != "") std::cout << c << "Root: " << locations[j].root << std::endl;
      if (locations[j].autoindex)
        std::cout << c << "Autoindex: on" << std::endl;
      else
        std::cout << c << "Autoindex: off" << std::endl;
      if (locations[j].methods.size() > 0) {
        std::cout << c << "Methods: ";
        for (unsigned long k = 0; k < locations[j].methods.size(); k++) {
          std::cout << locations[j].methods[k] << " ";
        }
        std::cout << std::endl;
      }
      if (locations[j].return_code != 0)
        std::cout << c << "Redirect: " << locations[j].return_code << " " << locations[j].return_url << std::endl;
      if (locations[j].index.size() > 0) {
        std::cout << c << "Index: ";
        for (unsigned long k = 0; k < locations[j].index.size(); k++) {
          std::cout << locations[j].index[k] << " ";
        }
        std::cout << std::endl;
      }
    }
    // go one step deeper
    if (locations[j].locations.size() > 0) {
      // layer_num.push_back(1);
      printLocations(locations[j].locations, layer + 1, detailed, layer_num);
    }
    layer_num.back()++;
  }
}

int ConfigFileParser::testFunction(std::string key, std::vector<std::string>& args, int& line_count) {
  int i;
  if (key == "server")
    i = server(args, line_count, 0);
  else if (key == "listen")
    i = listen(args, line_count, 1);
  else if (key == "server_name")
    i = serverName(args, line_count, 1);
  else if (key == "error_page")
    i = errorPage(args, line_count, 1);
  else if (key == "client_max_body_size")
    i = clientMaxBodySize(args, line_count, 1);
  else if (key == "location")
    i = location(args, line_count, 1);
  else if (key == "limit_except")
    i = limitExcept(args, line_count, 2);
  else if (key == "return")
    i = returnKeyword(args, line_count, 2);
  else if (key == "root")
    i = root(args, line_count, 2);
  else if (key == "autoindex")
    i = autoindex(args, line_count, 2);
  else if (key == "index")
    i = index(args, line_count, 2);
  else
    i = 2;
  return i;
}

} /* namespace configfile */