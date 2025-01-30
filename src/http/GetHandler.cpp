#include "http/GetHandler.hpp"

#include "http/Router.hpp"

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
		const std::string& filePath = request.getUri().path;

		if (Router::fileExists(filePath)) {
			response.setStatusCode(NOT_FOUND); // or FORBIDDEN, or maybe something else...
			return handleError(request, response);
		}

		std::ifstream inFile(filePath.c_str(), std::ios::binary);
		if (!inFile.is_open()) {
			response.setStatusCode(FORBIDDEN);
			return handleError(request, response);
		}

		std::stringstream buffer;
		buffer << inFile.rdbuf();
		response.setBody(buffer.str());
		response.setHeader("Content-Length", shared::string::to_string(buffer.str().size()));
		response.setStatusCode(OK);
	}

} /* namespace http */
