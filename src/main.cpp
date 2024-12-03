#include <iostream>

#include "configfile/Parser.hpp"
#include "logger/LoggerManager.hpp"
#include "www/CoreServer.hpp"

int main(int argc, char** const argv) {
  try {
    logger::LoggerManager& loggerManager = logger::LoggerManager::getInstance();
    loggerManager.registerLogger("Console", new logger::ConsoleLogger());
    loggerManager.setLoggerLevel("Console", logger::ERROR);
    loggerManager.registerLogger("File", new logger::FileLogger());
    loggerManager.setLoggerLevel("File", logger::DEBUG);
    loggerManager.setGlobalLogLevel(logger::DEBUG);
    LOG("test log...", logger::FATAL);
    // start
    std::string configfile;
    if (argc > 1) {
      configfile = argv[1];
    } else {
      configfile = "config/configfile_example";
    }
    www::CoreServer server;
    configfile::ConfigFileParser configFileParser;
    if (configFileParser.loadConfigFile(configfile) == 1) return 1;

    try {
      server.init();
      configfile::t_config_data configData = configFileParser.getConfigData();
      server.addVirtualServers(configData);
      // server.getVirtualServers().at(0).addConnection();
    } catch (std::exception& e) {
      std::cerr << "Exception caught in main: " << e.what() << std::endl;
      return 1;
    }
    // end
    loggerManager.unregisterLogger("File");
    loggerManager.unregisterLogger("Console");
  } catch (const std::exception& e) {
    LOG("ERROR: " << e.what(), logger::ERROR)
  }
}
