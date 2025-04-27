#include "core/RequestProcessor.hpp"

#include "core/ARequestHandler.hpp"
#include "core/DeleteRequestHandler.hpp"
#include "core/GetRequestHandler.hpp"
#include "core/PostRequestHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

namespace core {

	RequestProcessor::RequestProcessor(io::Dispatcher& dispatcher)
		: m_cgiProcessor(dispatcher)
		, m_response(NULL) {
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

	// todo maybe have a isComplete function. the return value could be confusing (also applies to CGIProcessor, Request/Response Parser)
	bool RequestProcessor::processRequest(const http::Request* request) {
		if (!m_response) {
			m_response = new http::Response();
		}

		if (request->isValid() == false) { // yeah this is kinda weird...
			generateErrorResponse(request->getStatusCode());
			return false;
		}

		try {
			http::Request::Type requestType = request->getType();

			if (requestType == http::Request::FETCH) {
				ARequestHandler* handler = m_handlers[request->getMethod()];
				if (handler->handle(request, m_response)) {
					return true;
				}
				m_response->appendBody("tmp response\n"); // tmp
			} else if (requestType == http::Request::CGI) {
				if (m_cgiProcessor.process(*request)) {
					return true;
				}
				delete m_response;
				m_response = m_cgiProcessor.releaseResponse();
			}
		} catch (const http::HttpException& e) {
			LOG_ERROR("failed to process request: " + std::string(e.what()));
			generateErrorResponse(e.getStatusCode());
			return false;
		} catch (const std::exception& e) {
			LOG_ERROR("failed to process request: " + std::string(e.what()));
			generateErrorResponse(http::INTERNAL_SERVER_ERROR);
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

		for (HandlerMap::iterator it = m_handlers.begin(); it != m_handlers.end(); ++it) {
			it->second->reset();
		}

		m_cgiProcessor.reset();
	}

	void RequestProcessor::generateErrorResponse(http::StatusCode) {
		m_response->appendBody("tmp error response\n"); // tmp
	}

} // namespace core
