#include "http/GoodRouter.hpp"

#include <sys/stat.h>
#include <unistd.h>

namespace http {

	/**
	 * @brief Constructs a new GoodRouter object.
	 */
	GoodRouter::GoodRouter(const std::vector<config::Location>& locations, const config::Location& globalRoot)
		: m_locations(locations)
		, m_globalRoot(globalRoot) {}

	/**
	 * @brief Destroys the GoodRouter object.
	 */
	GoodRouter::~GoodRouter() {}

	GoodRouter::GoodRouter(const GoodRouter& other)
		: m_locations(other.m_locations)
		, m_globalRoot(other.m_globalRoot) {}

	const GoodRouter& GoodRouter::operator=(const GoodRouter& rhs) {
		if (this == &rhs) {
			return *this;
		}
		m_locations = rhs.m_locations;
		m_globalRoot = rhs.m_globalRoot;
		return *this;
	}

	const config::Location& GoodRouter::getGlobalRoot() const {
		return m_globalRoot;
	}

	//

	FileType GoodRouter::checkFileType(const std::string& absolutePath) {
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

	std::vector<std::string> GoodRouter::splitPath(const std::string& path) throw(http::exception) {
		std::vector<std::string> tokens;
		std::cout << "Splitting path: " << path << std::endl;
		if (path.empty()) {
			throw http::exception(NOT_FOUND, "Path is empty");
		}
		if (path[0] != '/') {
			throw http::exception(BAD_REQUEST, "Path doesn't begin with '/'");
		}

		std::stringstream ss(path.substr(1)); // Remove leading '/'
		std::string segment;
		while (std::getline(ss, segment, '/')) {
			if (!segment.empty()) {
				tokens.push_back(segment);
			}
		}
		return tokens;
	}

	std::string GoodRouter::joinPath(const std::vector<std::string>& pathComponents) {
		std::string joined = "";
		for (std::vector<std::string>::const_iterator it = pathComponents.begin(); it != pathComponents.end(); ++it) {
			joined += "/";
			joined += *it;
		}
		return joined;
	}

	std::vector<std::string> GoodRouter::normalizePath(const std::vector<std::string>& path) {
		std::vector<std::string> normalized;

		for (std::vector<std::string>::const_iterator it = path.begin(); it != path.end(); ++it) {
			if (*it == "..") {
				if (!normalized.empty()) {
					normalized.pop_back();
				}
			} else if (!it->empty() && *it != ".") {
				normalized.push_back(*it);
			}
		}
		return normalized;
	}

	std::string GoodRouter::findAbsolutePath(const config::Location& location, const std::string& subPath) {
		std::string absolutePath = joinPath(m_globalRoot.root) + joinPath(location.root);
		return absolutePath + subPath;
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
	// TODO: handle autoindex somewhere
	std::pair<std::string, const config::Location*> GoodRouter::routeToPath(
		const std::vector<std::string>& uriPath,		 // the requests' path that we're traversing
		const config::Location& currentLocation,		 // Location we're currently inside of, default should be m_globalRoot
		const std::vector<std::string>& currentRootPath, // nearest 'root', default should be m_globalRoot.path
		std::size_t redirects,
		std::size_t depth) throw(http::exception) {
		if (depth > MAX_REDIRECTS) {
			throw http::exception(LOOP_DETECTED, "Depth exceeded MAX_REDIRECTS");
		}
		if (redirects > MAX_REDIRECTS) {
			throw http::exception(LOOP_DETECTED, "Redirects exceeded MAX_REDIRECTS");
		}
		if (currentLocation.hasRedirect()) {
			std::cout << "Redirect hit!" << std::endl;
			return routeToPath(currentLocation.redirectUriTokens, m_globalRoot, m_globalRoot.path, redirects + 1); // TODO: invalid, this would then always return globalRoot's route
		}																										   // TODO: how the frick do we solve this?
		if (uriPath.size() <= depth) {
			return std::make_pair(joinPath(m_globalRoot.root) + joinPath(currentRootPath), &currentLocation); // TODO: I think this doesn't set the root path properly yet, does it?
		}
		for (std::vector<config::Location>::const_iterator loc = currentLocation.locations.begin(); loc != currentLocation.locations.end(); ++loc) {
			std::cout << "Checking whether " << loc->path[0] << " and " << uriPath.at(depth) << " are equal..." << std::endl;
			if (loc->path[0] == uriPath.at(depth)) {
				std::cout << "Location matched: " << loc->path[0] << std::endl;
				const std::vector<std::string>& nextRootPath = !loc->root.empty() ? loc->root : currentRootPath; // TODO: assign inherited rootPath during parsing
				return routeToPath(uriPath, *loc, nextRootPath, redirects, depth + 1);							 // use nearest parent
			}
		}
		std::vector<std::string> subDirectory(uriPath.begin() + depth, uriPath.end());
		std::cout << "currentRootPath is: " << joinPath(currentRootPath) << std::endl;
		std::cout << "currentLocation.root is: " << joinPath(currentLocation.root) << std::endl;
		if (currentLocation.root.empty()) {
			if (currentRootPath == m_globalRoot.root) { // TODO: alternatively, give each location its predefined absolute path after parsing
				return std::make_pair(joinPath(currentRootPath) + joinPath(subDirectory), &currentLocation);
			}
			return std::make_pair(joinPath(m_globalRoot.root) + joinPath(currentRootPath) + joinPath(subDirectory), &currentLocation);
		} else {
			if (currentLocation.root == m_globalRoot.root) {
				return std::make_pair(joinPath(currentLocation.root) + joinPath(subDirectory), &currentLocation);
			}
			return std::make_pair(joinPath(m_globalRoot.root) + joinPath(currentLocation.root) + joinPath(subDirectory), &currentLocation);
		}
	}
} // namespace http
