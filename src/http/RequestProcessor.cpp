#include "http/RequestProcessor.hpp"

#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"

#include <algorithm>

namespace http {

	/**
	 * @brief Constructs a new RequestProcessor object.
	 */
	RequestProcessor::RequestProcessor(Router& router)
		: m_res(NULL)
		, m_router(router) {
		m_handlers.insert(std::make_pair(GET, new GetHandler(m_router)));
		m_handlers.insert(std::make_pair(POST, new PostHandler(m_router)));
		m_handlers.insert(std::make_pair(DELETE, new DeleteHandler(m_router)));
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

	Response* RequestProcessor::process(Request& req) {
		if (!req.hasError()) {
			try {
				const config::Location& globalRoot = m_router.getGlobalRoot();
				std::pair<std::string, const config::Location*> location = m_router.routeToPath(req.getUri().pathSegments, globalRoot, globalRoot.root);
				std::cout << "path returned: " << location.first << std::endl;
				FileType fileType = Router::checkFileType(location.first);
				if (fileType == _NOT_FOUND) {
					throw http::exception(NOT_FOUND, "File or directory not found");
				}
				if (location.second->allowedMethods.find(req.getMethod()) == location.second->allowedMethods.end()) { // TODO: probably shouldn't check this here, causes files that
					throw http::exception(METHOD_NOT_ALLOWED, "HTTP method not allowed for this route");			  //       access root or its subdirectories to always return 405
				}																									  // TODO: also double-check whether allowed methods should cascade
				req.getUri().safeAbsolutePath = location.first;
				req.setLocation(location.second);
			} catch (const http::exception& e) {
				std::cout << "CRUD, " << e.getMessage() << std::endl;
				req.setStatusCode(e.getCode());
			}
		}

		m_res = new Response();

		// 	// TODO: ensure m_res doesn't leak in case of another exception being thrown in here

		if (!req.hasError()) {
			try {
				m_handlers[req.getMethod()]->handle(req, *m_res); // might throw a http::exception
			} catch (const http::exception& e) {
				req.setStatusCode(e.getCode());
			}
		}
		if (req.hasError()) {
			m_res->setStatusCode(req.getStatusCode());
			m_handlers[req.getMethod()]->handleError(*m_res);
		}
		return this->releaseResponse();
	}

	Response* RequestProcessor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
	}
} /* namespace http */
