#include "http/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include "http/RequestProcessor.hpp"

namespace http {
	/**
	 * @brief Constructs a new Router object.
	 */
	Router::Router(const std::vector<config::Location>& locations, const config::Location& globalRoot, const std::string& dataDir)
		: m_locations(locations)
		, m_globalRoot(globalRoot)
		, m_dataDir(dataDir) {}

	/**
	 * @brief Destroys the Router object.
	 */
	Router::~Router() {}

	Router::Router(const Router& other)
		: m_locations(other.m_locations)
		, m_globalRoot(other.m_globalRoot)
		, m_dataDir(other.m_dataDir) {}

	const Router& Router::operator=(const Router& rhs) {
		if (this == &rhs) {
			return *this;
		}
		m_locations = rhs.m_locations;
		m_globalRoot = rhs.m_globalRoot;
		m_dataDir = rhs.m_dataDir;
		return *this;
	}

	/**
	 * @brief Splits a normalized path into tokens.
	 *
	 * @param path The normalized URI path.
	 * @param tokens Vector to store the split tokens.
	 * @return int 0 on success, 1 on failure.
	 */
	void Router::splitPath(const std::string& path, std::vector<std::string>& tokens) {
		// Return 1 if the path is invalid in the current context
		// For example, we may require that the path always starts with '/'
		if (path.empty()) {
			throw http::exception(NOT_FOUND, "Path is empty");
		}
		if (path[0] != '/') {
			throw http::exception(BAD_REQUEST, "Path doesn't begin with '/'");
		}

		std::stringstream ss(path.substr(1)); // Remove leading '/'
		std::string segment;
		while (std::getline(ss, segment, '/')) {
			// TODO: decide to allow empty segments or treat them as invalid
			// probably just accept it / normalize it away
			if (!segment.empty()) {
				tokens.push_back(segment);
			}
		}
	}

	/**
	 * @brief  ursively finds the deepest matching location for a normalized path.
	 *
	 * @param normUri The normalized URI path.
	 * @param locs The current level's locations.
	 * @return const config::Location* The deepest matching location or NULL if none found.
	 */
	const config::Location* Router::locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs) {
		// Convert the normalized URI to tokens
		std::vector<std::string> uriTokens;
		splitPath(normUri, uriTokens); // throws http::exception if path isn't valid

		const config::Location* bestLocation = NULL;
		size_t bestMatchLength = 0;

		for (std::vector<config::Location>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
			// Combine it->path into a string to compare with uriTokens
			// or directly compare the location path tokens if they're stored in a vector
			const std::vector<std::string>& locTokens = it->path;
			size_t matchedCount = 0;

			// Compare tokens
			while (matchedCount < locTokens.size() && matchedCount < uriTokens.size() && locTokens[matchedCount] == uriTokens[matchedCount]) {
				matchedCount++;
			}

			// If full location path was matched and it’s the deepest match so far
			if (matchedCount == locTokens.size() && matchedCount > bestMatchLength) {
				// Check if there are nested children that match deeper
				if (!it->locations.empty()) {
					std::string subUri = "/";
					for (size_t j = matchedCount; j < uriTokens.size(); j++) {
						subUri += uriTokens[j] + "/";
					}
					// recurse into children
					std::cout << "locating at: " << subUri << std::endl;
					const config::Location* deeperLoc = locateDeepestMatch(subUri, it->locations);
					if (deeperLoc) {
						bestLocation = deeperLoc;
						bestMatchLength = matchedCount + deeperLoc->path.size();
						continue;
					}
				}
				// If no deeper match, set this as best location
				bestLocation = &(*it);
				bestMatchLength = matchedCount;
			}
		}

		return bestLocation;
	}

	/**
	 * @brief Builds the absolute base root for the uri.
	 *
	 * @param uri The target uri.
	 * @return std::string The absolute path to the best-matching Location's root.
	 * i.e. if global_root is /var/foo, data_directory is /www, uri.path is /bar/baz and there's a location /bar/baz with root /qux/quux
	 * it should return "/var/foo/www/qux/quux"
	 * If there is no route match at all, it should simply return global_root
	 * @throws http::exception If the path escapes the base root or is invalid.
	 */
	std::string Router::getBaseRoot(const Uri& uri) {
		const config::Location& location = getLocation(uri);
		if (location.root == m_globalRoot.root) {
			return location.root + m_dataDir;
		}
		return m_globalRoot.root + m_dataDir + location.root; // returned path should not have '/' at the end
															  // TODO: ensure location.root always starts with '/',
															  // never ends with '/' and never contains double '/'s
															  // TODO: ensure the same for dataDir and globalRoot
	}

	/**
	 * @brief Builds the final absolute path, ensuring it does not escape the base root.
	 *
	 * @param baseRoot The base root directory.
	 * @param normUri The normalized URI path.
	 * @return std::string The final absolute file path.
	 * @throws http::exception If the path escapes the base root or is invalid.
	 */
	std::string Router::getSafePath(const Uri& uri) {
		const std::string& baseRoot = getBaseRoot(uri); // getBaseRoot(const Uri& uri) -> returns <global_root>/<uri.root>
		const std::string& normalizedUri = normalizePath(uri.path);
		const bool isFile = uri.path.length() > 0 ? uri.path.at(uri.path.size() - 1) != '/' : false; // TODO: this sucks

		// Merge baseRoot and normUri while ensuring we never go above baseRoot
		// First normalize the baseRoot
		if (baseRoot.empty() || baseRoot[0] != '/') {
			return "/";
		}

		std::string combined = baseRoot;
		// Ensure baseRoot does not end with '/'
		if (combined.at(combined.size() - 1) == '/') {
			combined.erase(combined.end() - 1);
		}

		// Ensure normUri starts with '/'
		std::string adjustedNormUri = normalizedUri;
		if (adjustedNormUri.empty() || adjustedNormUri[0] != '/') {
			adjustedNormUri = "/" + adjustedNormUri;
		}

		combined += adjustedNormUri;

		// Split the combined path into components
		std::vector<std::string> components;
		shared::string::splitPath(combined, components);

		// Resolve the path components
		std::vector<std::string> resolvedComponents;
		for (std::vector<std::string>::iterator it = components.begin(); it != components.end(); ++it) {
			if (*it == "..") {
				if (!resolvedComponents.empty()) {
					resolvedComponents.pop_back();
				}
			} else if (*it != "." && !it->empty()) {
				resolvedComponents.push_back(*it);
			}
		}

		// Rebuild the resolved path
		std::string resolvedPath = "/";
		for (std::vector<std::string>::iterator it = resolvedComponents.begin(); it != resolvedComponents.end(); ++it) {
			resolvedPath += *it + "/";
		}

		// Ensure the final path does not end with a slash if it's a file
		if (isFile) {
			std::cout << "path isn't dir?" << std::endl;
			if (resolvedPath.at(resolvedPath.size() - 1) == '/') {
				resolvedPath.erase(resolvedPath.end() - 1);
			}
		}

		// Ensure the final path is within the base root
		if (resolvedPath.find(baseRoot) != 0) {
			throw http::exception(FORBIDDEN, "Path escapes the base root: " + resolvedPath);
		}

		std::cout << "getSafePath returned: " << resolvedPath << std::endl;
		return resolvedPath;
	}

	// really, this function needs to:
	// - figure out if the path is a file (should only be GET) or location
	// - figure out if that location exists
	// - if POSTing a file, if that location is writeable
	// - if GETing or DELETEing a file, if that file exists and is accessible
	// - treat server root (<serverLocation>/<data_dir>) as global root // server root must be defined, as we can't use getcwd()
	// - block '../' escaping the global root directory
	// TODO: ensure /cgi-bin/<script.cgi> path is also normalized so cgi-bin/ can't be escaped
	std::string Router::normalizePath(const std::string& uriPath) {
		std::vector<std::string> tokens;
		splitPath(uriPath, tokens); // handle empty tokens, etc.
		std::vector<std::string> normalized;

		for (size_t i = 0; i < tokens.size(); i++) {
			if (tokens[i] == "..") {
				if (!normalized.empty()) {
					normalized.pop_back();
				}
			} else if (!tokens[i].empty() && tokens[i] != ".") {
				normalized.push_back(tokens[i]);
			}
		}
		// Rebuild path
		std::string result = "/";
		for (size_t i = 0; i < normalized.size(); i++) {
			result += normalized[i] + "/";
		}
		return result;
	}

	// TODO: route and return location
	// or throw error, maybe should reuse that pattern
	// NOTE: I would expect this to:
	// - Traverse redirects and stuff
	// - Return the best matching Location, if any matches
	// - Return the subdirectory of that matching Location, if no subroute matches the rest of the path
	// - Maybe also check whether that path even exists in our file system?
	// - Maybe another function should handle this though
	/**
	 * @brief Routes the given URI to the corresponding location, handling redirects.
	 *
	 * @param uri The request URI.
	 * @return const config::Location& The matched location.
	 * @throws http::exception on errors such as too many redirects or invalid paths.
	 */
	const config::Location& Router::getLocation(const http::Uri& uri) {
		const int MAX_REDIRECTS = 10;
		int redirectCount = 0;
		http::Uri currentUri = uri;
		const config::Location* currentLocation = NULL;

		while (redirectCount < MAX_REDIRECTS) {
			std::string normPath = normalizePath(currentUri.path);
			if (normPath.empty()) {
				throw http::exception(FORBIDDEN, "Invalid normalized path"); // maybe bad request instead
			}
			currentLocation = locateDeepestMatch(normPath, m_locations);
			if (currentLocation == NULL) {
				return m_globalRoot;
				// TODO: return a default location somehow... ugh
				// probably should be the location for <global_root>/www/<subroute>
				// which should always exist and be accessible
				// meaning localhost:8080/foo/bar would attempt to locate
				// <global_root>/www/foo/bar
				// alternatively, always throw 404/500 if no locations are defined
				// or rather, if no global_root is defined
			}
			if (currentLocation->hasRedirect()) {
				std::string redirectPath = currentLocation->redirectUrl;
				// Validate redirect path
				// TODO: maybe do this during parsing instead? idk
				if (redirectPath.empty() || redirectPath[0] != '/') {
					throw http::exception(INTERNAL_SERVER_ERROR, "Invalid redirect path in location");
				}
				currentUri.path = redirectPath;
				redirectCount++;
				continue;
			}
			// Found the final location
			break;
		}

		if (redirectCount >= MAX_REDIRECTS) {
			throw http::exception(LOOP_DETECTED, "Too many redirects");
		}
		// TODO: actually detect loops by keeping track of routes traversed?
		return *(currentLocation);
	}

	// TODO: route (perhaps using getLocation?) and confirm whether file exists
	// TODO: alternatively return a StatusCode for OK, FORBIDDEN, NOT_FOUND, etc.
	// TODO: or just throw?
	/**
	 * @brief Checks if a file exists and is a regular file.
	 *
	 * @param filePath The path to the file.
	 * @return StatusCode Status indicating if the file exists (OK) or not (NOT_FOUND).
	 */
	StatusCode Router::fileExists(const std::string& absoluteFilePath) {
		struct stat st;
		if (stat(absoluteFilePath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
			return OK;
		}
		return NOT_FOUND;
	}

	/**
	 * @brief prints a location.
	 * @param locations the locations to print.
	 * @param detailed toggles between detailed and simple output. (0 for simple, 1
	 * for detailed)
	 */
	void Router::printLocation(const config::Location& location, int detailed) {
		std::cout << "--------------------------" << std::endl;
		std::cout << "Location: ";
		for (std::vector<std::string>::const_iterator it = location.path.begin();
			 it != location.path.end();
			 ++it) {
			std::cout << *it << "/";
		}
		std::cout << std::endl;
		if (detailed) {
			if (location.root != "")
				std::cout << "Root: " << location.root << std::endl;
			if (location.autoindex)
				std::cout << "Autoindex: on" << std::endl;
			else
				std::cout << "Autoindex: off" << std::endl;
			if (location.methods.size() > 0) {
				std::cout << "Methods: ";
				for (unsigned long k = 0; k < location.methods.size();
					 k++) {
					std::cout << location.methods[k] << " ";
				}
				std::cout << std::endl;
			}
			if (location.index.size() > 0) {
				std::cout << "Index: ";
				for (unsigned long k = 0; k < location.index.size(); k++) {
					std::cout << location.index[k] << " ";
				}
				std::cout << std::endl;
			}
		}
		std::cout << "--------------------------" << std::endl;
	}

} /* namespace http */
