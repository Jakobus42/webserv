#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "http/Request.hpp"

#include <iostream>

int signal_handler(int signum) {
	return signum;
}

int main(int argc, char** const argv) {

	try {
		std::string dummyReq =
			"POST /login HTTP/1.1\n"
			"Host: example.com\n"
			"Content-Type: application/json\n"
			"Authorization: Bearer some-jwt-token\n"
			"User-Agent: MyCustomClient/1.0\n"
			"Content-Length: 34\n"
			"\n"
			"{\"username\": \"user\", \"password\": \"pass123\"}";





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
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Server died :(" << std::endl;
		return 1;
	}
	return 0;
}
