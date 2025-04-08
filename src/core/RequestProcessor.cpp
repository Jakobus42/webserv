#include "core/RequestProcessor.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/http.hpp"

namespace core {

	RequestProcessor::RequestProcessor() {
	}

	RequestProcessor::~RequestProcessor() {
	}

	http::Response* generateErrorResponse(http::StatusCode statusCode) {
		http::Response* response = new http::Response();
		response->setStatusCode(statusCode);
		response->setReasonPhrase(http::statusCodeToMessage(statusCode));
		return response;
	}

	http::Response* RequestProcessor::processRequest(http::Request*) {
		return new http::Response();
	}

} /* namespace core */
