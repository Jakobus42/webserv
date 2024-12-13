#include "config/Parser.hpp"
#include "core/Reactor.hpp"

#include <iostream>

int main(int argc, char **const argv) {
	http::Request request;
	std::string input = "GET / HTTP/1.1\nHost: localhost:8080, abcde\nUser-Agent: curl/7.68.0\nAccept: */*\nContent-Length: 30\n\n 1234123213567890";
	//std::cout << "input1: " << input.substr(0, 5) << std::endl;
	for (size_t i = 0; i < input.size(); i += 5) {
		request.setReadBuffer(input.substr(i, 5).c_str());
		request.parse();
	}
	request.PrintRequestData();
	

	
	
	
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
