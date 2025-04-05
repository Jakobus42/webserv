#include "config/Parser.hpp"

int main() {
	config::Parser configParser;
	if (!configParser.parseFile("config/config.conf")) {
		return 1;
	}
	// std::vector<config::Server>& serverConfig = configParser.getConfigs();
	// for (std::size_t i = 0; i < serverConfig.size(); ++i) {
	// 	serverConfig[i].print();
	// }
	return 0;
}