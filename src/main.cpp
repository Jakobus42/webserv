#include <iostream>

#include "configfile/Parser.hpp"
#include "logger/LoggerManager.hpp"
#include "www/CoreServer.hpp"
#include "www/Router.hpp"

void foo(const http::Request&, www::Response&) {}

int main(int argc, char** const argv) {
  www::CoreServer server;
  www::Router router;

  try {
    server.init();
    server.addVirtualServer("Server1", 8080, ONE_MEGABYTE);
  } catch (std::exception& e) {
    std::cerr << "Exception caught in main: " << e.what() << std::endl;
    return 1;
  }

  std::cout << "Added server: ";
  std::cout << server.getVirtualServers().at(0).getName() << std::endl;
  server.getVirtualServers().at(0).getSocket().close();

  // if (server.getVirtualServers().at(0).addConnection() == false) {
  //   std::cout << "Oh darnit, adding connection to VServer failed" <<
  //   std::endl;
  // }

  try {
    logger::LoggerManager& loggerManager = logger::LoggerManager::getInstance();
    loggerManager.registerLogger("Console", new logger::ConsoleLogger());
    loggerManager.setLoggerLevel("Console", logger::DEBUG);
    loggerManager.registerLogger("File", new logger::FileLogger());
    loggerManager.setLoggerLevel("File", logger::DEBUG);
    loggerManager.setGlobalLogLevel(logger::DEBUG);
    std::string configfile;
    if (argc > 1) {
      configfile = argv[1];
    } else {
      configfile = "config/configfile_example";
    }
    configfile::ConfigFileParser configFileParser;
    if (configFileParser.loadConfigFile(configfile) == 1) return 1;
    router.registerRoute(www::Route("foo", www::GET), foo);

    www::Response res;
    router.handleRequest(http::Request(), res);

    // configFileParser.printConfigData(1);
    loggerManager.unregisterLogger("File");
    loggerManager.unregisterLogger("Console");

  } catch (const std::exception& e) {
    LOG("ERROR: " << e.what(), logger::ERROR)
  }
}
