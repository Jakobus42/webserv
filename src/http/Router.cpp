#include "http/Router.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include "http/RequestProcessor.hpp"

namespace http {

	std::vector<config::Location> Router::m_locations; // TODO: ensure this is initialized
	config::Location Router::m_defaultLocation;		   // TODO: currently not initialized

	/**
	 * @brief Constructs a new Router object.
	 */
	Router::Router() {}

	/**
	 * @brief Destroys the Router object.
	 */
	Router::~Router() {}

	bool Router::splitPath(const std::string& path, std::vector<std::string>& tokens) {
		if (path.empty() || path[0] != '/') {
			return false; // Invalid path
		}

		std::stringstream ss(path.substr(1)); // Remove leading '/'
		std::string segment;
		while (std::getline(ss, segment, '/')) {
			if (!segment.empty()) {
				tokens.push_back(segment);
			}
		}
		return true;
	}

	const config::Location* Router::locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs) {
		std::vector<std::string> uriTokens;
		if (splitPath(normUri, uriTokens) == false) {
			return NULL;
		}

		const config::Location* bestLocation = NULL;
		size_t bestMatchLength = 0;

		for (std::vector<config::Location>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
			const std::vector<std::string>& locTokens = it->path;
			size_t matchedCount = 0;

			// Compare tokens
			while (matchedCount < locTokens.size() && matchedCount < uriTokens.size() && locTokens[matchedCount] == uriTokens[matchedCount]) {
				matchedCount++;
			}

			// Check if all tokens in location's path matched
			if (matchedCount == locTokens.size() && matchedCount > bestMatchLength) {
				// Recurse into children with remaining URI tokens
				if (!it->locations.empty()) {
					std::string subUri = "/";
					for (size_t j = matchedCount; j < uriTokens.size(); j++) {
						subUri += uriTokens[j] + "/";
					}
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

	std::string Router::buildFinalPath(const std::string& baseRoot, const std::string& normUri) {
		std::string combined = baseRoot;

		// Ensure baseRoot does not end with '/'
		if (!combined.empty() && combined.at(combined.size() - 1) == '/') {
			combined.erase(combined.end() - 1);
		}

		// Ensure normUri starts with '/'
		std::string adjustedNormUri = normUri;
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

	// TODO: route and return location
	// or throw error, maybe should reuse that pattern
	// config::Location& Router::getLocation(const http::Uri& uri) {

	// }
	const config::Location& Router::getLocation(const http::Uri& uri) {
		const int MAX_REDIRECTS = 10;
		int redirectCount = 0;
		http::Uri currentUri = uri;
		const config::Location* currentLocation = NULL;

		while (redirectCount < MAX_REDIRECTS) {
			std::string normPath = RequestProcessor::normalizePath(currentUri.path);
			if (normPath.empty()) {
				throw http::exception(FORBIDDEN, "Invalid normalized path"); // maybe bad request instead
			}
			currentLocation = locateDeepestMatch(normPath, m_locations);
			if (currentLocation == NULL) {
				return m_defaultLocation;
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
		return *(currentLocation);
	}

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
