#include "http/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

namespace http {

	/**
	 * @brief Constructs a new Router object.
	 */
	Router::Router() {}

	/**
	 * @brief Destroys the Router object.
	 */
	Router::~Router() {}

	// TODO: refactor to void, throwing http::exception
	int Router::splitPath(const std::string& path, std::vector<std::string>& tokens) {
		if (path.empty() || path[0] != '/') {
			return 1; // Invalid path
		}

		std::stringstream ss(path.substr(1)); // Remove leading '/'
		std::string segment;
		while (std::getline(ss, segment, '/')) {
			if (!segment.empty()) {
				tokens.push_back(segment);
			}
		}
		return 0;
	}

	// TODO: route and return location
	// or throw error, maybe should reuse that pattern
	// config::Location& Router::getLocation(const http::Uri& uri) {

	// }

	// TODO: route (perhaps using getLocation?) and confirm whether file exists
	// TODO: alternatively return a StatusCode for OK, FORBIDDEN, NOT_FOUND, etc.
	// TODO: or just throw?
	StatusCode Router::fileExists(const std::string& filePath) {
		struct stat st;
		if (stat(filePath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
			return OK;
		}
		return NOT_FOUND;
	}

} /* namespace http */
