#include "config/Parser.hpp"

int main(int argc, char** argv) {
	config::Parser configParser;
	if (!configParser.parseFile(argc == 2 ? argv[1] : "config/config.conf")) {
		return 1;
	}
	return 0;
}