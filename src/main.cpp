#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "shared/Logger.hpp"

#include <iostream>

int main(/* int argc, char* argv[] */) {
	try {
		// config::Parser configParser;
		// if (configParser.parseFile(argc > 1 ? argv[1] : "config/config.conf")) { // this could also just throw
		// 	return 1;
		// }

		/* tmp */

		config::Config config;
		std::vector<std::map<std::string, config::ServerConfig> > listenServerConfigs;
		std::map<std::string /*server_name*/, config::ServerConfig> serverConfigs;

		config::ServerConfig server1;
		server1.dataDirectory = "/workspaces/webserv";
		server1.serverNames.push_back("cool-server.com");

		config::LocationConfig location1;
		location1.root = "/www/foo";
		location1.indexFile.push_back("index.html");

		server1.location = location1;
		server1.print();

		config::ServerConfig server2;
		server2.dataDirectory = "/workspaces/webserv";
		server2.serverNames.push_back("cool-server2.com");

		config::LocationConfig location2;
		location2.root = "/www/bar";
		location2.indexFile.push_back("index.html");

		server2.location = location2;
		server2.print();

		// todo: change serverNames to serverName
		serverConfigs[server1.serverNames.front()] = server1;
		serverConfigs[server2.serverNames.front()] = server2;

		config.serverConfigs_ = listenServerConfigs;

		/* tmp end */

		core::Reactor reactor;
		reactor.init(config);
		reactor.run();
	} catch (const std::exception& e) {
		LOG_FATAL(e.what());
		return 1;
	} catch (...) {
		return 1;
	}
	return 0;
}