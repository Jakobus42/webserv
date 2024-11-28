#include "../../include/configfile/Parser.hpp"

namespace configfile {

/**
 * @brief parses a line in the configuration file.
 * @param line the line to parse.
 * @param layer the current layer of the configuration file.
 * @param line_count the current line number.
 * @return int 0 if successful, 1 if not.
 */
int ConfigFileParser::handlePrompt(std::string& line, int layer, int& line_count) {
  // return 0 if successful and semicolon is found, 1 if semicolon is not found, 2 if error
  // cut off trailing and starting spaces
  int qoute_flag = 1;
  while (line[line.size() - 1] == ' ') {
    line.erase(line.size() - 1);
  }
  while (line[0] == ' ') {
    line.erase(0, 1);
  }
  if (line[line.size() - 1] == ';') {
    qoute_flag = 0;
    line.erase(line.size() - 1);
  }
  if (line.find(';') != std::string::npos) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Missplaced semicolon (;) found" << std::endl,
        logger::ERROR)
    return 2;
  }
  std::vector<std::string> args;
  std::stringstream stream(line);
  std::string key;
  while (getline(stream, key, ' ')) {
    args.push_back(key);
  }
  if (SaveConfigData(args, layer, qoute_flag, line_count) == 1) {
    return 2;
  }
  return qoute_flag;
}

/**
 * @brief finds the current location in the configuration file.
 *
 * @param layer the current layer of the configuration file.
 * @return struct location* the current location.
 */
t_location* ConfigFileParser::getLocation(int layer) {
  configfile::t_location* temp = &m_configData.servers.back().locations.back();
  for (int i = 2; i < layer; i++) {
    temp = &temp->locations.back();
  }
  return temp;
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
           it != m_configData.servers[i].errorPages.end(); ++it) {
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
void ConfigFileParser::printLocations(const std::vector<configfile::t_location>& locations, int layer, int detailed,
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

int ConfigFileParser::testFunction(std::string const& key, std::vector<std::string>& args, int& line_count) {
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
    return 2;
  return i;
}

/**
 * @brief Identifies the command in the configuration file. and returns the id of the command.
 * @param key The key to identify.
 * @return The id of the command. (1 if listen, 2 if server_name, 3 if error_page, 4 if client_max_body_size, 5 if
 * location, 6 if limit_exept, 7 if return, 8 if root, 9 if autoindex, 10 if index, 404 if not found.)
 */
enum cmd_id ConfigFileParser::idCommand(std::string const& key) {
  if (key == "server")
    return server_id;
  else if (key == "listen")
    return listen_id;
  else if (key == "server_name")
    return server_name_id;
  else if (key == "error_page")
    return error_page_id;
  else if (key == "client_max_body_size")
    return client_max_body_size_id;
  else if (key == "location")
    return location_id;
  else if (key == "limit_except")
    return limit_except_id;
  else if (key == "return")
    return return_id;
  else if (key == "root")
    return root_id;
  else if (key == "autoindex")
    return autoindex_id;
  else if (key == "index")
    return index_id;
  else
    return unknown_id;
}

/**
 * @brief Saves the configuration data.
 * @param args The arguments to save.
 * @param layer The layer of the configuration file.
 * @param qoute_flag The flag to check if quotes are found. 0 if found, 1 if not found.
 * @param line_count The current line number.
 * @return 0 if successful, 1 if error.
 */
int ConfigFileParser::SaveConfigData(std::vector<std::string>& args, int layer, int qoute_flag, int& line_count) {
  enum cmd_id command_id = idCommand(args[0]);
  if (command_id == unknown_id) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "Unknown command found" << std::endl,
        logger::ERROR)
    return 1;
  }
  if (layer == 0)  // global
  {
    if (qoute_flag == 0) {
      LOG("Configuration file (line " << line_count << "): "
                                      << "Unexpected command found" << std::endl,
          logger::ERROR)
      return 1;
    } else {
      if (command_id == server_id) {
        if (server(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    }
  } else if (layer == 1)  // server
  {
    if (qoute_flag == 0) {
      if (command_id == listen_id) {
        if (listen(args, line_count, layer) == 1) return 1;
      } else if (command_id == server_name_id) {
        if (serverName(args, line_count, layer) == 1) return 1;
      } else if (command_id == error_page_id) {
        if (errorPage(args, line_count, layer) == 1) return 1;
      } else if (command_id == client_max_body_size_id) {
        if (clientMaxBodySize(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    } else {
      if (command_id == location_id) {
        if (location(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    }
  } else if (layer > 1)  // location
  {
    if (qoute_flag == 0) {
      if (command_id == limit_except_id) {
        if (limitExcept(args, line_count, layer) == 1) return 1;
      } else if (command_id == return_id) {
        if (returnKeyword(args, line_count, layer) == 1) return 1;
      } else if (command_id == root_id) {
        if (root(args, line_count, layer) == 1) return 1;
      } else if (command_id == autoindex_id) {
        if (autoindex(args, line_count, layer) == 1) return 1;
      } else if (command_id == index_id) {
        if (index(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    } else {
      if (command_id == location_id) {
        if (location(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    }
  }
  return 0;
}

} /* namespace configfile */