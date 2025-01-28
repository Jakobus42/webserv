#include "http/ErrorHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new ErrorHandler object.
	 */
	ErrorHandler::ErrorHandler(const config::Location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the ErrorHandler object.
	 */
	ErrorHandler::~ErrorHandler() {
	}

	void ErrorHandler::handle(const Request& request, Response& response) {
		response.setStatusCode(IM_A_TEAPOT);
		response.setBody(getErrorPage(request.getStatusCode()));
		response.setHeader("Content-Length", shared::string::fromNum(response.getBody().length()));
		response.setHeader("Content-Type", TEXT_HTML);
	}

} /* namespace http */
