#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "http/ErrorMessages.hpp"

#include <iostream>

int main(int argc, char** const argv) {

	try {
		http::ErrorMessages::eagerInit();

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
/* 		http::Request request;
		std::string input = "GET / HTTP/1.1\r\nContent-Length: 30\r\nHost: localhost:8080, abcde\r\n\r\nUser-Agent: curl/7.68.0\r\nAccept: wdadd\r\n\r\n 1234123213567890";
		std::string input3 = "POST / HTTP/1.1\r\nHost: www.example.com\r\nTransfer-Encoding: chunked\r\nContent-Type:text/plain\r\n\r\n 5\r\nHello\r\n6\r\nWorlde\r\n0\r\n";
		std::string input2 = "POST / HTTP/1.1\r\nHost: www.example.com\r\nTransfer-Encoding: chunked\r\nContent-Type:text/plain\r\n\r\n1E\r\nHello World! How are you today\r\n\r\n1E\r\nHello World! How are you today\r\n0\r\n";
		int bytes = 1;
		for (size_t i = 0; i < input.size(); i += bytes) {
			request.setReadBuffer(input.substr(i, bytes).c_str());
			if (!request.parse()) 
			{
				std::cout << "Failed to parse request" << std::endl;
				return 1;
			}
		} 
	request.PrintRequestData();*/
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}
