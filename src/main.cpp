#include "config/GoodParser.hpp"
#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "shared/Logger.hpp"

#include <iostream>

int main(int argc, char** const argv) {
	std::vector<config::Server> configs;

	try {
		std::string configPath = argc > 1 ? argv[1] : "config/configfile_example";
		config::GoodParser parser;
		if (!parser.parseFile(configPath)) {
			return 69;
		}
		configs = parser.getConfigs();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Parsing failed, exiting..." << std::endl;
	}

	for (std::vector<config::Server>::iterator server = configs.begin(); server != configs.end(); ++server) {
		server->print();
	}

	return 42;

	try {
		// config::Parser configParser;
		// if (configParser.loadConfig(configPath) == 1)
		// 	return 1;
		// const std::vector<config::ServerConfig>& serverConfigs = configParser.getServerConfigs();

		// core::Reactor reactor;
		// reactor.init(serverConfigs);
		// reactor.react();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Server died :(" << std::endl;
		return 1;
	}
	return 0;
}
