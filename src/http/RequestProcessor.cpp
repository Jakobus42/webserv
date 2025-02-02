#include "http/RequestProcessor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

#include <algorithm>

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
	// TODO: check for allowed methods
	Response* RequestProcessor::process(Request& req) {

		if (!req.hasError()) {
			try {
				const config::Location& location = m_router.getLocation(req.getUri());
				std::cout << "Looking for request method " << req.getMethod() << " (root " << location.root << ")" << std::endl;
				std::cout << "Allowed methods (" << location.allowedMethods.size() << "): ";
				for (std::set<Method>::iterator it = location.allowedMethods.begin(); it != location.allowedMethods.end(); ++it) {
					std::cout << *it << " ";
				}
				std::cout << std::endl;
				if (location.allowedMethods.find(req.getMethod()) == location.allowedMethods.end()) {
					throw http::exception(METHOD_NOT_ALLOWED, "HTTP method not allowed for this route");
				}
			} catch (const http::exception& e) {
				std::cout << "CRUD, " << e.getMessage() << std::endl;
				req.setStatusCode(e.getCode());
			}
		}

		m_res = new Response();

		if (req.hasError()) {
			m_res->setStatusCode(req.getStatusCode());
			m_handlers[req.getMethod()]->handleError(*m_res);
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

	// TODO: unused
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

	// TODO: delete this
	// /**
	//  * @brief Builds the final absolute path, preventing escape from root.
	//  */
	// std::string buildFinalPath(const std::string& baseRoot, const std::string& normUri) {
	// 	// Merge baseRoot and normUri while ensuring we never go above baseRoot
	// 	// First normalize the baseRoot
	// 	if (baseRoot.empty() || baseRoot[0] != '/') {
	// 		return "/";
	// 	}
	// 	// Combine baseRoot and normUri
	// 	std::string combined = baseRoot;
	// 	if (combined[combined.size() - 1] == '/') {
	// 		combined.resize(combined.size() - 1);
	// 	}
	// 	// normUri is already normalized, so just append
	// 	combined += normUri;

	// 	// Now split combined to ensure no leftover ".." escapes
	// 	std::vector<std::string> tokens;
	// 	splitPath(combined, tokens); // reuse your splitPath
	// 	// Rebuild path from tokens
	// 	std::string safePath = "/";
	// 	for (size_t i = 0; i < tokens.size(); i++) {
	// 		safePath += tokens[i] + "/";
	// 	}
	// 	return safePath;
	// }

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
