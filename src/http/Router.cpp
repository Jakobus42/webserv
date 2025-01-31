#include "http/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include "http/RequestProcessor.hpp"

namespace http {
	/**
	 * @brief Constructs a new Router object.
	 */
	Router::Router(const std::vector<config::Location>& locations, const std::string& globalRoot)
		: m_locations(locations)
		, m_globalRoot(globalRoot) {}

	/**
	 * @brief Destroys the Router object.
	 */
	Router::~Router() {}

	Router::Router(const Router& other)
		: m_locations(other.m_locations)
		, m_globalRoot(other.m_globalRoot) {}

	const Router& Router::operator=(const Router& rhs) {
		if (this == &rhs) {
			return *this;
		}
		m_locations = rhs.m_locations;
		m_globalRoot = rhs.m_globalRoot;
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
	 * @brief Builds the final absolute path, ensuring it does not escape the base root.
	 *
	 * @param baseRoot The base root directory.
	 * @param normUri The normalized URI path.
	 * @return std::string The final absolute file path.
	 * @throws http::exception If the path escapes the base root or is invalid.
	 */
	std::string Router::getSafePath(const std::string& baseRoot, const std::string& normalizedUri) {
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
		// normUri is already normalized, so just append

		// Now split combined to ensure no leftover ".." escapes
		// Ensure normUri starts with '/'
		std::string adjustedNormUri = normalizedUri;
		if (adjustedNormUri.empty() || adjustedNormUri[0] != '/') {
			adjustedNormUri = "/" + adjustedNormUri;
		}

		combined += adjustedNormUri;

		// Resolve the combined path to an absolute path
		char real_combined[PATH_MAX];
		if (realpath(combined.c_str(), real_combined) == NULL) {
			throw http::exception(BAD_REQUEST, "Invalid path: " + combined + " - " + std::strerror(errno));
		}

		// Resolve the base root to an absolute path
		char real_root[PATH_MAX];
		if (realpath(baseRoot.c_str(), real_root) == NULL) {
			throw http::exception(INTERNAL_SERVER_ERROR, "Invalid base root: " + baseRoot + " - " + std::strerror(errno));
		}

		std::string s_real_combined(real_combined);
		std::string s_real_root(real_root);

		// Ensure the final path is within the base root
		if (s_real_combined.find(s_real_root) != 0) {
			throw http::exception(FORBIDDEN, "Path escapes the base root: " + s_real_combined);
		}

		return s_real_combined;
	}

	// really, this function needs to:
	// - figure out if the path is a file (should only be GET) or location
	// - figure out if that location exists
	// - if POSTing a file, if that location is writeable
	// - if GETing or DELETEing a file, if that file exists and is accessible
	// - treat server root (<serverLocation>/www/) as global root
	// - block '../' escaping the global root directory
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
				return m_globalRoot; // TODO: return a default location somehow... ugh
									 //       probably should be the location for <global_root>/www/<subroute>
									 //       which should always exist and be accessible
									 // 	  meaning localhost:8080/foo/bar would attempt to locate
									 // 	  <global_root>/www/foo/bar
									 //       alternatively, always throw 404/500 if no locations are defined
									 //       or rather, if no global_root is defined
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
