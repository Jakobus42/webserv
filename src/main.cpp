#include "config/Parser.hpp"
#include "core/Reactor.hpp"
#include "shared/Logger.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
	try {
		config::Parser configParser;
		if (configParser.parseFile(argc > 1 ? argv[1] : "config/config.conf")) { // this could also just throw
			return 1;
		}

		core::Reactor reactor;
		reactor.init(configParser.getConfig());
		reactor.run();
	} catch (const std::exception& e) {
		LOG_FATAL(e.what());
		return 1;
	} catch (...) {
		return 1;
	}
	return 0;
}