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
		, m_handlers()
		, m_router(router)
		, m_done(false)
		, m_logger() {
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

	void RequestProcessor::log(const std::string& msg, shared::LogLevel level, const Request& request) {
		std::string formatted = "[" + getMethodString(request.getMethod()) + " " + request.getUri().path + "] ";

		formatted += msg;
		m_logger.log(formatted, level);
	}

	void RequestProcessor::handleError(Response* response) {
		// if (response.getLocation().errorPages matches response.getStatusCode()) {
		//   if file exists (matched code to errorPage) {
		//     read file and return that
		//} else {
		// proceed as usual
		//}
		//}
		response->setBody(getErrorPage(response->getStatusCode()));
		response->setHeader("Content-Length", shared::string::toString(response->getBody().length()));
		response->setHeader("Content-Type", TEXT_HTML);
	}

	void RequestProcessor::routeToSafePath(Request& req) {
		const config::Location& serverRoot = m_router.getServerRoot();
		std::pair<std::string, const config::Location*> location = m_router.routeToPath(req.getUri().pathSegments, serverRoot);

		this->log("routeToSafePath returned: " + location.first, shared::DEBUG, req);

		req.setFileType(Router::checkFileType(location.first));

		if (req.getFileType() == _NOT_FOUND) {
			throw http::exception(NOT_FOUND, "File or directory not found");
		}
		if (!location.second->allowedMethods.empty() && location.second->allowedMethods.find(req.getMethod()) == location.second->allowedMethods.end()) { // TODO: probably shouldn't check this here, causes files that
			throw http::exception(METHOD_NOT_ALLOWED, "HTTP method not allowed for this route");														  //       access root or its subdirectories to always return 405
		}																																				  // TODO: also double-check whether allowed methods should cascade

		req.getUri().safeAbsolutePath = location.first;
		req.setLocation(location.second);
	}

	void RequestProcessor::process(Request& req) {
		if (!m_res) {
			m_res = new Response();
		}
		ARequestHandler* handler = m_handlers[req.getMethod()];

		try {
			if (req.hasError()) {
				throw http::exception(req.getStatusCode());
			}
			if (req.needsSafePath()) {
				routeToSafePath(req);
			}
			handler->handle(req, *m_res);
			m_done = handler->isComplete();
		} catch (const http::exception& e) {
			this->log("CRAP, " + e.getMessage() + "; handling error now", shared::ERROR, req);
			m_res->setStatusCode(e.getStatusCode());
			handleError(m_res);
			m_done = true;
		}
		if (m_done) {
			handler->reset();
		}
	}

	Response* RequestProcessor::releaseResponse() {
		Response* released = m_res;
		m_res = NULL;
		return released;
	}

	bool RequestProcessor::isDone() const { return m_done; }

	void RequestProcessor::reset() {
		for (std::map<Method, ARequestHandler*>::iterator handler = m_handlers.begin(); handler != m_handlers.end(); ++handler) {
			handler->second->reset();
		}
		delete m_res;
		m_res = NULL;
		m_done = false;
	}

} /* namespace http */
