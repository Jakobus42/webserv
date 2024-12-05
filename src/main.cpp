#include <iostream>

#include "config/Parser.hpp"
#include "logger/LoggerManager.hpp"
#include "www/Reactor.hpp"

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
    www::Reactor server;
    config::ConfigFileParser configFileParser;
    if (configFileParser.loadConfigFile(configfile) == 1) return 1;

    try {
      server.init();
      config::t_config_data configData = configFileParser.getConfigData();
      if (server.addVirtualServers(configData) == false) return 1;
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
