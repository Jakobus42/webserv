#include <algorithm>
#include <iostream>

#include "../include/logger/LoggerManager.hpp"
#include "../include/utils/StringUtils.hpp"

int main(void) {
  try {
    logger::LoggerManager& loggerManager = logger::LoggerManager::getInstance();
    loggerManager.registerLogger("Console", new logger::ConsoleLogger());

    std::ifstream config("config/nginxSample0.conf");
    std::string content;

    if (!config.is_open()) throw std::runtime_error("cant find config");
    config.seekg(0, std::ios::end);
    content.resize(config.tellg());
    config.seekg(0, std::ios::beg);
    config.read(&content[0], content.size());

    std::vector<std::string> splittedContent;
    utils::string::split(content, splittedContent, "\n");
    for (std::vector<std::string>::iterator it = splittedContent.begin(); it != splittedContent.end(); ++it) {
      std::cout << "'" << *it << "'" << std::endl;
    }

  } catch (const std::exception& e) {
    LOG("ERROR: " << e.what(), logger::ERROR)
  }
}
