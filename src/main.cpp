#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "http/Request.hpp"
#include "http/RequestParser.hpp"
#include "shared/stringUtils.hpp"

#include <iostream>

int signal_handler(int signum) {
	return signum;
}

int main(int argc, char** const argv) {

	std::deque<std::size_t> lol;

	try {
		std::string chunkReq =
			"POST /upload HTTP/1.1\r\n"
			"Host:         example.com  ,        foo   ,        bar           \r\n"
			"Transfer-Encoding: chunked\r\n"
			"User-Agent: MyCustomClient/1.0\r\n"
			"Authorization: Bearer some-jwt-token\r\n"
			"\r\n"
			"7\r\n"
			"Hello, \r\n"
			"5\r\n"
			"world!\r\n"
			"0\r\n"
			"\r\n";

		std::string dummyReq =
			"POST /upload HTTP/1.1\r\n"
			"Host: exa'mple.com     asda     , fooo, bar\r\n"
			"Content-Length: 13\r\n"
			"User-Agent: MyCustomClient/1.0\r\n"
			"Authorization: Bearer some-jwt-token\r\n"
			"\r\n"
			"Hello, world!";

		std::string malformed_req =
			"GET / HTTP/1.1\r\n"
			"Host: example.coET / HTTP/1.1\r\n"
			"Host: e\r\n"
			" HTTP/1.1\r\n"
			"Hom\r\n"
			"\r\n";

		const http::Request* req;

		{

			http::RequestParser reqParser;

			shared::Buffer<BUFFER_SIZEE>& writeBuffer = reqParser.getWriteBuffer();
			writeBuffer.append(dummyReq.c_str(), dummyReq.length());
			reqParser.process();

			req = reqParser.releaseRequest();
		}

		std::cout << std::endl
				  << req->toString() << std::endl;

		delete req;

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
