#include "../../include/configfile/Parser.hpp"
namespace configfile {

/**
 * @brief Constructs a new ConfigFileParser object.
 */
ConfigFileParser::ConfigFileParser() { m_isLoaded = 0; }

/**
 * @brief Destroys the ConfigFileParser object.
 */
ConfigFileParser::~ConfigFileParser() {}

/**
 * @brief Copy constructor.
 * @param other The other ConfigFileParser object to copy.
 */
ConfigFileParser::ConfigFileParser(const ConfigFileParser&) { m_isLoaded = 0; }

/* *
 * @brief Constructs a new ConfigFileParser object.
 * @param configFileName The name of the configuration file.
 */
/* ConfigFileParser::ConfigFileParser(std::string &configFileName)
{
        loadConfigFile(configFileName);
} */

/**
 * @brief Copy assignment operator.
 * @param other The other ConfigFileParser object to assign from.
 * @return A reference to the assigned ConfigFileParser object.
 */
ConfigFileParser& ConfigFileParser::operator=(const ConfigFileParser&) {
  m_isLoaded = 0;
  return *this;
}

/**
 * @brief Reads the configuration file.
 * @param configFileName The name of the configuration file.
 */
std::string ConfigFileParser::readConfigFile(std::string& configFileName) {
  if (configFileName.empty()) {
    LOG("Error: Configuration file name is empty." << std::endl, logger::ERROR)
    return std::string();
  }
  std::string line;
  std::ifstream infile;
  infile.open(configFileName.c_str());
  char c;
  if (!infile.is_open()) {
    LOG("Error: Could not open configuration file." << std::endl, logger::ERROR)
    return std::string();
  }
  while (!infile.eof() && infile >> std::noskipws >> c) {
    if (c == '#') {
      while (c != '\n') {
        infile >> std::noskipws >> c;
      }
      line += '\n';
    } else {
      line += c;
    }
  }
  infile.close();
  if (infile.is_open()) {
    LOG("Error: Could not close Configuration file" << std::endl, logger::ERROR)
    return std::string();
  }
  if (line.empty()) {
    LOG("Error: Configuration file is empty." << std::endl, logger::ERROR)
    return std::string();
  }
  return line;
}

/**
 * @brief Identifies the command in the configuration file. and returns the id of the command.
 * @param key The key to identify.
 * @return The id of the command. (1 if listen, 2 if server_name, 3 if error_page, 4 if client_max_body_size, 5 if
 * location, 6 if limit_exept, 7 if return, 8 if root, 9 if autoindex, 10 if index, 404 if not found.)
 */
int ConfigFileParser::idCommand(std::string const& key) {
  if (key == "server")
    return 0;
  else if (key == "listen")
    return 1;
  else if (key == "server_name")
    return 2;
  else if (key == "error_page")
    return 3;
  else if (key == "client_max_body_size")
    return 4;
  else if (key == "location")
    return 5;
  else if (key == "limit_except")
    return 6;
  else if (key == "return")
    return 7;
  else if (key == "root")
    return 8;
  else if (key == "autoindex")
    return 9;
  else if (key == "index")
    return 10;
  else
    return 404;
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
  int command_id = idCommand(args[0]);
  if (command_id == 404) {
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
      if (command_id == 0) {
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
      if (command_id == 1) {
        if (listen(args, line_count, layer) == 1) return 1;
      } else if (command_id == 2) {
        if (serverName(args, line_count, layer) == 1) return 1;
      } else if (command_id == 3) {
        if (errorPage(args, line_count, layer) == 1) return 1;
      } else if (command_id == 4) {
        if (clientMaxBodySize(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    } else {
      if (command_id == 5) {
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
      if (command_id == 6) {
        if (limitExcept(args, line_count, layer) == 1) return 1;
      } else if (command_id == 7) {
        if (returnKeyword(args, line_count, layer) == 1) return 1;
      } else if (command_id == 8) {
        if (root(args, line_count, layer) == 1) return 1;
      } else if (command_id == 9) {
        if (autoindex(args, line_count, layer) == 1) return 1;
      } else if (command_id == 10) {
        if (index(args, line_count, layer) == 1) return 1;
      } else {
        LOG("Configuration file (line " << line_count << "): "
                                        << "Unexpected command found" << std::endl,
            logger::ERROR)
        return 1;
      }
    } else {
      if (command_id == 5) {
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
 * @brief Parses the configuration file.
 * @param str The configuration file as a string.
 * @param layer The layer of the configuration file in our current pos.
 * @param i The current position in the configuration file.
 * @param line_count The current line number.
 * @return 0 if successful, 1 if error.
 */
int ConfigFileParser::parseConfigFile(std::string& str, int layer, unsigned long& i, int& line_count) {
  std::string line;
  int find_quotes_flag = 0;
  for (; i < str.size(); i++) {
    if (str[i] == '\n') {
      line_count++;
      if (line.empty() || line.find_first_not_of(' ') == std::string::npos) {
        line.clear();
        continue;
      } else if (line.find('}') != std::string::npos) {
        // check if the line contains anything else than brackets or whitespace
        if (line.find_first_not_of(" }") != std::string::npos) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Quotes must be in an otherwise empty line" << std::endl,
              logger::ERROR)
          return 1;
        }
        // check if there are multiple brackets on the same line
        if (line.find('}') != line.find_last_of('}')) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Multiple brackets on the same line" << std::endl,
              logger::ERROR)
          return 1;
        }
        if (layer == 0) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Unexpected closing bracket found" << std::endl,
              logger::ERROR)
          return 1;
        } else {
          return 0;
        }
      } else if (line.find('{') != std::string::npos) {
        // find if the line contains anything else than brackets or whitespace
        if (line.find_first_not_of(" {") != std::string::npos) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Quotes must be in an otherwise empty line" << std::endl,
              logger::ERROR)
          return 1;
        }
        // check if there are multiple brackets on the same line
        if (line.find('{') != line.find_last_of('{')) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Multiple brackets on the same line" << std::endl,
              logger::ERROR)
          return 1;
        }
        // find if quotes are expected here
        if (find_quotes_flag == 0) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Quotes not expected at this point in file" << std::endl,
              logger::ERROR)
          return 1;
        } else {
          find_quotes_flag = 0;
        }
        i++;
        if (parseConfigFile(str, layer + 1, i, line_count) == 1) {
          return 1;
        }
      } else {
        if (find_quotes_flag == 1) {
          LOG("Configuration file (line " << line_count << "): "
                                          << "Missing semicolon at the end of line" << std::endl,
              logger::ERROR)
          return 1;
        }
        int ret = handlePrompt(line, layer, line_count);
        if (ret == 2) {
          return 1;
        }
        find_quotes_flag = ret;
      }
      line.clear();
    } else {
      line += str[i];
    }
  }
  return 0;
}

/**
 * @brief Loads the configuration file, saving all the information.
 * @param configFileName The name of the configuration file.
 * @return 0 if successful, 1 if error.
 */
int ConfigFileParser::loadConfigFile(std::string& configFileName) {
  std::string file = readConfigFile(configFileName);
  if (file.empty()) {
    return 1;
  }
  unsigned long i = 0;
  int line_count = 0;
  for (unsigned long b = 0; b < file.size(); b++) {
    if (file[b] == '\t' || file[b] == '\r' || file[b] == '\v' || file[b] == '\f') file[b] = ' ';
  }
  file += '\n';
  if (parseConfigFile(file, 0, i, line_count) == 1) {
    m_configData.servers.clear();
    return 1;
  }
  if (m_configData.servers.size() == 0) {
    LOG("Configuration file (line " << line_count << "): "
                                    << "No servers found in configuration file" << std::endl,
        logger::ERROR)
    return 1;
  }
  m_isLoaded = 1;
  return 0;
}

int ConfigFileParser::getIsLoaded() const { return m_isLoaded; }

Config_data ConfigFileParser::getConfigData() const { return m_configData; }

} /* namespace configfile */
