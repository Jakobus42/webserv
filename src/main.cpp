#include <algorithm>
#include <iostream>

#include "../include/logger/LoggerManager.hpp"
#include "../include/utils/StringUtils.hpp"
#include "../include/configfile/Parser.hpp"

int main(void) {
  try {
    logger::LoggerManager& loggerManager = logger::LoggerManager::getInstance();
    loggerManager.registerLogger("Console", new logger::ConsoleLogger());
    loggerManager.setLoggerLevel("Console", logger::ERROR);
    loggerManager.registerLogger("File", new logger::FileLogger());
    loggerManager.setLoggerLevel("File", logger::DEBUG);
    loggerManager.setGlobalLogLevel(logger::DEBUG);
    loggerManager.unregisterLogger("File");
    loggerManager.unregisterLogger("Console");
    LOG("test log...", logger::FATAL);
  } catch (const std::exception& e) {
    LOG("ERROR: " << e.what(), logger::ERROR)
  }
	std::string configfile = "config_files/configfile_example";
	configfile::ConfigFileParser configFileParser(configfile);
	configFileParser.print_config_data(0);
}
