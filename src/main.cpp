#include "config/Parser.hpp"
#include "core/Reactor.hpp"

#include <iostream>

int main(int argc, char **const argv) {
	try {
		// start
		std::string configfile;
		if (argc > 1) {
			configfile = argv[1];
		} else {
			configfile = "config/configfile_example";
		}
		core::Reactor server;
		config::ConfigFileParser configFileParser;
		if (configFileParser.loadConfigFile(configfile) == 1)
			return 1;

		try {
			server.init();
			config::t_config_data configData = configFileParser.getConfigData();
			if (server.addVirtualServers(configData) == false)
				return 1;
			// server.getVirtualServers().at(0).addConnection();
		} catch (std::exception &e) {
			std::cerr << "Exception caught in main: " << e.what() << std::endl;
			return 1;
		}
		// end
	} catch (const std::exception &e) {
		LOG("ERROR: " << e.what(), 1)
	}
}
