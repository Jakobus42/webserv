#include "http/RequestProcessor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new RequestProcessor object.
	 */
	// todo figure out how to do this shit with locations
	RequestProcessor::RequestProcessor(Router& router)
		: m_res(NULL)
		, m_router(router) {
		m_handlers.insert(std::make_pair(GET, new GetHandler(m_router)));
		m_handlers.insert(std::make_pair(POST, new PostHandler(m_router)));
		m_handlers.insert(std::make_pair(DELETE, new DeleteHandler(m_router))); // todo router or idk:c
	}

	/**
	 * @brief Destroys the RequestProcessor object.
	 */
	RequestProcessor::~RequestProcessor() {
		for (std::map<Method, ARequestHandler*>::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_res;
	}

	// todo check if req was valid - if not send error response
	// todo check for allowed methods
	Response* RequestProcessor::process(Request& req) {
		m_res = new Response();

		if (!req.hasError()) {
			try {
				// m_router.findLocation(req.getUri().path, location); // currently not implemented, probably replaced with getLocation()? TODO: verify
				const config::Location& location = m_router.getLocation(req.getUri());
				// do something with the location????? xd
			} catch (const http::exception& e) {
				req.setStatusCode(e.getCode());
				std::cout << "FindLocation failed; Location not found :(" << std::endl;
			}
		}

		if (req.hasError()) {
			m_handlers[req.getMethod()]->handleError(req, *m_res);
		} else {
			m_handlers[req.getMethod()]->handle(req, *m_res);
		}
		return this->releaseResponse();
	}

	Response* RequestProcessor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
	}

	int comparePaths(const std::vector<std::string>& LocationPath, const std::vector<std::string>& uriPath, int& len) {
		unsigned long paths = 0;
		if (LocationPath.size() > uriPath.size()) {
			return 1;
		}
		if (LocationPath.size() == 0 && uriPath.size() != 0) {
			return 1;
		}
		for (std::vector<std::string>::const_iterator it = LocationPath.begin(); it != LocationPath.end(); ++it) {
			if (*it != uriPath[paths]) {
				return 1;
			}
			paths++;
		}
		len = paths;
		return 0;
	}

	int splitPath(const std::string& path, std::vector<std::string>& result) {
		// Return 1 if the path is invalid in your context
		// For example, you may require that the path always starts with '/'
		if (path.empty()) {
			throw http::exception(NOT_FOUND, "Path is empty");
		}
		if (path[0] != '/') {
			throw http::exception(BAD_REQUEST, "Path doesn't begin with '/'");
		}

		std::stringstream ss(path.substr(1));
		std::string segment;
		while (std::getline(ss, segment, '/')) {
			if (!segment.empty()) {
				result.push_back(segment);
			}
		}
		return 0;
	}

	/**
	 * @brief Recursively finds the deepest matching location for a normalized path.
	 *        You could store nested locations in config::Location::children or similar.
	 */
	const config::Location* locateDeepestMatch(const std::string& normUri, const std::vector<config::Location>& locs) {
		std::vector<std::string> uriTokens;
		if (splitPath(normUri, uriTokens) != 0) {
			return NULL;
		}

		const config::Location* bestLocation = NULL;
		size_t bestMatchLength = 0;

		// For each location, see how many tokens match
		for (std::vector<config::Location>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
			// Combine it->path into a string to compare with uriTokens
			// or directly compare the location path tokens if they're stored in a vector
			std::vector<std::string> locTokens = it->path; // e.g. /foo/bar => {"foo","bar"}
			size_t matchedCount = 0;

			while (matchedCount < locTokens.size() && matchedCount < uriTokens.size() && locTokens[matchedCount] == uriTokens[matchedCount]) {
				matchedCount++;
			}

			if (matchedCount == locTokens.size() && matchedCount > bestMatchLength) {
				if (!it->locations.empty()) {
					// Rebuild sub-URI from the unmatched tail
					std::string subUri = "/";
					for (size_t j = matchedCount; j < uriTokens.size(); j++) {
						subUri += uriTokens[j] + "/";
					}
					// Recurse into children
					std::cout << "locating at: " << subUri << std::endl;
					const config::Location* deeperLoc = locateDeepestMatch(subUri, it->locations);
					if (deeperLoc) {
						bestLocation = deeperLoc;
						bestMatchLength = matchedCount + deeperLoc->path.size();
						continue;
					}
				}
				bestLocation = &(*it);
				bestMatchLength = matchedCount;
			}
		}
		return bestLocation;
	}

	/**
	 * @brief Builds the final absolute path, preventing escape from root.
	 */
	std::string buildFinalPath(const std::string& baseRoot, const std::string& normUri) {
		// Merge baseRoot and normUri while ensuring we never go above baseRoot
		// First normalize the baseRoot
		if (baseRoot.empty() || baseRoot[0] != '/') {
			return "/";
		}
		// Combine baseRoot and normUri
		std::string combined = baseRoot;
		if (combined[combined.size() - 1] == '/') {
			combined.resize(combined.size() - 1);
		}
		// normUri is already normalized, so just append
		combined += normUri;

		// Now split combined to ensure no leftover ".." escapes
		std::vector<std::string> tokens;
		splitPath(combined, tokens); // reuse your splitPath
		// Rebuild path from tokens
		std::string safePath = "/";
		for (size_t i = 0; i < tokens.size(); i++) {
			safePath += tokens[i] + "/";
		}
		return safePath;
	}

	// really, this function needs to:
	// - figure out if the path is a file (should only be GET) or location
	// - figure out if that location exists
	// - if POSTing a file, if that location is writeable
	// - if GETing or DELETEing a file, if that file exists and is accessible
	// - treat server root (<serverLocation>/www/) as global root
	// - block '../' escaping the global root directory
	std::string RequestProcessor::normalizePath(const std::string& uriPath) {
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

	// int RequestProcessor::findLocation(const std::string& uri, const std::vector<config::Location>& locs, config::Location& location) {
	// 	std::string normUri = normalizePath(uri);

	// 	const config::Location* bestMatch = locateDeepestMatch(normUri, locs);
	// 	if (!bestMatch) {
	// 		return 1;
	// 	} // No matching location.

	// 	config::Location chosen = *bestMatch;
	// 	// 3) Append whatever path remains after location matching,
	// 	// ensuring we stay within the final root.
	// 	chosen.root = buildFinalPath(chosen.root, normUri);
	// 	std::cout << "BUILT PATH: " << chosen.root << std::endl;

	// 	location = chosen;
	// 	return 0;
	// }

} /* namespace http */
