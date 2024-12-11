#include "config/Parser.hpp"
#include "core/Reactor.hpp"

#include <iostream>

int main(int argc, char **const argv) {
	try {
		std::string configPath = argc > 1 ? argv[1] : "config/configfile_example";
		config::ConfigFileParser configFileParser;
		if (configFileParser.loadConfigFile(configPath) == 1)
			return 1;
		config::t_config_data configData = configFileParser.getConfigData();

		core::Reactor reactor;
		reactor.init();
		if (reactor.addVirtualServers(configData) == false)
			return 1;
		reactor.react();
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}
