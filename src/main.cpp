#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "http/Request.hpp"
#include "http/RequestParser.hpp"

#include <iostream>

int signal_handler(int signum) {
	return signum;
}

int main(int argc, char** const argv) {

	try {
		std::string dummyReq =
			"POST /login HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Content-Type: application/json\r\n"
			"Authorization: Bearer some-jwt-token\r\n"
			"User-Agent: MyCustomClient/1.0\r\n"
			"Content-Length: 34\r\n"
			"\r\n"
			"{\"username\": \"user\", \"password\": \"pass123\"}";

		http::RequestParser reqParser;

		reqParser.feed(dummyReq.c_str(), dummyReq.size());

		const http::Request& req = reqParser.getRequest();
		std::cout << req.toString() << std::endl;



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
