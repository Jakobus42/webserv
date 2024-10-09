#include <iostream>

#include "../include/logger/LoggerManager.hpp"

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
    std::cerr << e.what() << std::endl;
  }
}
