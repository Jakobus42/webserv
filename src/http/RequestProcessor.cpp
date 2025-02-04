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
	RequestProcessor::RequestProcessor(GoodRouter& router)
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
		std::string safePath = "";

		if (!req.hasError()) {
			try {
				const config::Location& globalRoot = m_router.getGlobalRoot();
				std::cout << "PathSegments: " << std::endl;
				for (std::vector<std::string>::iterator it = req.getUri().pathSegments.begin(); it != req.getUri().pathSegments.end(); ++it) {
					std::cout << *it << " ";
				}
				std::cout << std::endl;
				std::pair<std::string, const config::Location*> location = m_router.routeToPath(req.getUri().pathSegments, globalRoot, globalRoot.root);
				// std::cout << "Looking for request method " << req.getMethod() << " (root " << location.root << ")" << std::endl;
				std::cout << "Allowed methods (" << location.second->allowedMethods.size() << "): ";
				for (std::set<Method>::iterator it = location.second->allowedMethods.begin(); it != location.second->allowedMethods.end(); ++it) {
					std::cout << *it << " ";
				}
				std::cout << std::endl;
				std::cout << "path returned: " << location.first << std::endl;
				if (location.second->allowedMethods.find(req.getMethod()) == location.second->allowedMethods.end()) {
					throw http::exception(METHOD_NOT_ALLOWED, "HTTP method not allowed for this route");
				}
				req.getUri().safeAbsolutePath = location.first;
				std::cout << "safePath in RequestProcessor: " << safePath << std::endl;
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

} /* namespace http */
