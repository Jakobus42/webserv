#include "http/RequestProcessor.hpp"

#include "http/ARequestHandler.hpp"
#include "http/DeleteHandler.hpp"
#include "http/GetHandler.hpp"
#include "http/PostHandler.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"
#include "shared/Logger.hpp"

namespace http {

	RequestProcessor::RequestProcessor()
		: m_response(NULL) {
		m_handlers[GET] = new GetHandler();
		m_handlers[POST] = new PostHandler();
		m_handlers[DELETE] = new DeleteHandler();
	}

	RequestProcessor::~RequestProcessor() { delete m_response; }

	// todo maybe have a isComplete function. the return value could be confusing
	bool RequestProcessor::processRequest(Request* request) {
		if (!m_response) {
			m_response = new Response();
		}

		if (request->isValid() == false) { // yeah this is kinda weird...
			generateErrorResponse(request->getStatusCode());
			return false;
		}

		ARequestHandler* handler = m_handlers[request->getMethod()];
		try {
			handler->handle(request, m_response);
			if (handler->isDone() == false) {
				return true;
			}
			handler->reset();
		} catch (const HttpException& e) {
			LOG_ERROR("request handler failed: " + std::string(e.what()));
			generateErrorResponse(e.getStatusCode());
			return false;
		}

		return false;
	}

	Response* RequestProcessor::releaseResponse() {
		Response* released = m_response;
		m_response = NULL;
		return released;
	}

	void RequestProcessor::reset() {
		delete m_response;
		m_response = NULL;
	}

	void RequestProcessor::generateErrorResponse(StatusCode) {
	}

} // namespace http
