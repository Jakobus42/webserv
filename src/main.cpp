#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>

#include "../include/configfile/Parser.hpp"
#include "../include/logger/LoggerManager.hpp"
#include "../include/utils/StringUtils.hpp"
#include "../include/www/send.hpp"

int main(int argc, char** const argv) {
  try {
    logger::LoggerManager& loggerManager = logger::LoggerManager::getInstance();
    loggerManager.registerLogger("Console", new logger::ConsoleLogger());
    loggerManager.setLoggerLevel("Console", logger::ERROR);
    loggerManager.registerLogger("File", new logger::FileLogger());
    loggerManager.setLoggerLevel("File", logger::DEBUG);
    loggerManager.setGlobalLogLevel(logger::DEBUG);
    LOG("test log...", logger::FATAL);
    std::string configfile;
    if (argc > 1) {
      configfile = argv[1];
    } else {
      configfile = "config/configfile_example";
    }
    configfile::ConfigFileParser configFileParser;
    if (configFileParser.loadConfigFile(configfile) == 1) return 1;
    // configFileParser.printConfigData(1);
    loggerManager.unregisterLogger("File");
    loggerManager.unregisterLogger("Console");
  } catch (const std::exception& e) {
    LOG("ERROR: " << e.what(), logger::ERROR)
  }
  const int port = 8080;
  int returnValue = doKoolShit(port);
  return returnValue;
}
