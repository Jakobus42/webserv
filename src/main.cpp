#include "config/Parser.hpp"
#include "io/Dispatcher.hpp"

int main(int argc, char** argv) {
	config::Parser configParser;
	if (!configParser.parseFile(argc == 2 ? argv[1] : "config/config.conf")) {
		return 1;
	}
	io::Dispatcher dispatcher;
	dispatcher.dispatch();
	return 0;
}