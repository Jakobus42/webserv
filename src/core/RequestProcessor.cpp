#include "core/RequestProcessor.hpp"

#include "http/ARequestHandler.hpp"
#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"

namespace core {

	RequestProcessor::RequestProcessor()
		: m_response(NULL) {
		m_handlers[http::GET] = new http::GetHandler();
		m_handlers[http::POST] = new http::PostHandler();
		m_handlers[http::DELETE] = new http::DeleteHandler();
	}

	RequestProcessor::~RequestProcessor() { delete m_response; }

	// todo maybe have a isComplete function. the return value could be confusing
	bool RequestProcessor::processRequest(http::Request* request) {
		if (!m_response) {
			m_response = new http::Response();
		}

		if (request->isValid() == false) { // yeah this is kinda weird...
			generateErrorResponse(request->getStatusCode());
			return false;
		}

		http::ARequestHandler* handler = m_handlers[request->getMethod()];
		try {
			handler->handle(request, m_response);
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
