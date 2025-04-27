#include "core/RequestProcessor.hpp"

#include "core/ARequestHandler.hpp"
#include "core/DeleteRequestHandler.hpp"
#include "core/GetRequestHandler.hpp"
#include "core/PostRequestHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"

namespace core {

	RequestProcessor::RequestProcessor()
		: m_response(NULL) {
		m_handlers[http::GET] = new GetRequestHandler();
		m_handlers[http::POST] = new PostRequestHandler();
		m_handlers[http::DELETE] = new DeleteRequestHandler();
	}

	RequestProcessor::~RequestProcessor() {
		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			delete it->second;
		}
		delete m_response;
	}

	// todo maybe have a isComplete function. the return value could be confusing
	bool RequestProcessor::processRequest(http::Request& request) {
		if (!m_response) {
			m_response = new http::Response();
		}

		if (request.isValid() == false) { // yeah this is kinda weird...
			generateErrorResponse(request.getStatusCode());
			return false;
		}

		ARequestHandler* handler = m_handlers[request.getMethod()];
		try {
			handler->handle(request, *m_response);
			if (handler->isDone() == false) {
				return true;
			}
			handler->reset();
		} catch (const http::HttpException& e) {
			LOG_ERROR("request handler failed: " + std::string(e.what()));
			generateErrorResponse(e.getStatusCode());
			return false;
		}

		return false;
	}

	http::Response* RequestProcessor::releaseResponse() {
		http::Response* released = m_response;
		m_response = NULL;
		return released;
	}

	void RequestProcessor::reset() {
		delete m_response;
		m_response = NULL;
	}

	void RequestProcessor::generateErrorResponse(http::StatusCode) {
	}

} // namespace core
