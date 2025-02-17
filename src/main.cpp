#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "shared/Logger.hpp"

#include <iostream>

int main(int argc, char** const argv) {
	std::vector<config::Server> configs;

	try {
		std::string configPath = argc > 1 ? argv[1] : "config/configfile_example";
		config::Parser parser;
		if (!parser.parseFile(configPath)) {
			return 1;
		}
		configs = parser.getConfigs();
		for (std::vector<config::Server>::iterator server = configs.begin(); server != configs.end(); ++server) {
			std::cout << "Printing..." << std::endl;
			server->print();
		}
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Parsing failed, exiting..." << std::endl;
	}

	try {
		core::Reactor reactor;
		reactor.init(configs);
		reactor.react();
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Server died :(" << std::endl;
		return 1;
	}
	return 0;
}
