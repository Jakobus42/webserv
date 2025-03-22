#include "http/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include "shared/fileUtils.hpp"
#include "shared/stringUtils.hpp"

namespace http {

	/**
	 * @brief Constructs a new Router object.
	 */
	Router::Router(const config::Location& rootLocation)
		: m_rootLocation(rootLocation) {}

	/**
	 * @brief Destroys the Router object.
	 */
	Router::~Router() {}

	Router::Router(const Router& other)
		: m_rootLocation(other.m_rootLocation) {}

	const Router& Router::operator=(const Router& rhs) {
		if (this == &rhs) {
			return *this;
		}
		m_rootLocation = rhs.m_rootLocation;
		return *this;
	}

	const config::Location& Router::getServerRoot() const {
		return m_rootLocation;
	}

	//

	FileType Router::checkFileType(const std::string& absolutePath) {
		struct stat statBuf;
		if (stat(absolutePath.c_str(), &statBuf) != 0) {
			return _NOT_FOUND; // Path does not exist
		}
		if (S_ISDIR(statBuf.st_mode)) {
			return DIRECTORY;
		} else if (S_ISREG(statBuf.st_mode)) {
			return FILE;
		}
		return _NOT_FOUND; // Other types (not file/dir)
	}

	// sample GET /foo/bar/baz
	/************************
	 * root /var/server;
	 * location /foo {
	 * 	return /qux/quux;
	 * }
	 * location /qux {
	 * 	location /quux {
	 * 		root /quux;
	 * 	}
	 * }
	 ************************/
	// TODO: currently doesn't handle locations with more than one path segment, i.e. location /foo/bar
	// TODO: probably just disallow this in the parser
	//       apparently NGINX requires it but I don't want to implement this
	// TODO: if indexFile is defined when a route has finished matching, immediately look for that indexFile, read and respond with it
	// TODO: if no indexFile is defined, but autoindex is on, generate and return a directory listing
	// TODO: if no indexFile is defined and autoindex is off, return a 403 forbidden response
	std::pair<std::string, const config::Location*>
	Router::routeToPath(const std::vector<std::string>& pathToMatch,
						const config::Location& currentLocation, // Location we're currently inside of, default should be m_serverRoot
						std::size_t redirects,
						std::size_t depth) throw(http::exception) {
		if (depth > MAX_REDIRECTS) {
			throw http::exception(LOOP_DETECTED, "Depth exceeded MAX_REDIRECTS");
		}
		if (redirects > MAX_REDIRECTS) {
			throw http::exception(LOOP_DETECTED, "Redirects exceeded MAX_REDIRECTS");
		}
		if (currentLocation.hasRedirect()) { // restart routing with the new route? or actually send a redirect response?
			std::cout << "Redirecting to " << currentLocation.redirectUri << std::endl;
			return routeToPath(currentLocation.redirectUriAsTokens, m_rootLocation, redirects + 1, 0); // TODO: invalid, this would then always return serverRoot's route
		}																							   // TODO: how the frick do we solve this?
		if (depth >= pathToMatch.size()) {															   // TODO: what about files?
			return std::make_pair(currentLocation.precalculatedAbsolutePath, &currentLocation);		   // TODO: I think this doesn't set the root path properly yet, does it?
		}
		for (std::vector<config::Location>::const_iterator loc = currentLocation.locations.begin(); loc != currentLocation.locations.end(); ++loc) {
			if (!loc->pathAsTokens.empty() && loc->pathAsTokens[0] == pathToMatch[depth]) { // TODO: breaks if location is '/' -> check if (!loc->path.empty())
				return routeToPath(pathToMatch, *loc, redirects, depth + 1);				// use nearest parent
			}
		}
		// no location matched
		std::vector<std::string> subDirectory(pathToMatch.begin() + depth, pathToMatch.end());
		return std::make_pair(currentLocation.precalculatedAbsolutePath + shared::string::joinPath(subDirectory), &currentLocation);
	}

} // namespace http
