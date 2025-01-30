#include "http/GetHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new GetHandler object.
	 */
	GetHandler::GetHandler(const config::Location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the GetHandler object.
	 */
	GetHandler::~GetHandler() {
	}

	void GetHandler::handle(const Request& request, Response& response) {

		// if (!canOpenFile)
		if (false) {
			response.setBody(getErrorPage(NOT_FOUND)); // or FORBIDDEN, or maybe something else...
			return handleError(request, response);
		} else {
			std::ifstream inFile(request.getUri().path.c_str());
			std::stringstream buffer;

			buffer << inFile.rdbuf();
			response.setBody(buffer.str());
			response.setHeader("Content-Length", shared::string::to_string(buffer.str().length()));
			response.setStatusCode(FOUND);
		}
	}

} /* namespace http */
