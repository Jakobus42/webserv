#include "http/GetHandler.hpp"

#include "http/Router.hpp"

namespace http {

	/**
	 * @brief Constructs a new GetHandler object.
	 */
	GetHandler::GetHandler(Router& router)
		: ARequestHandler(router) {}

	/**
	 * @brief Destroys the GetHandler object.
	 */
	GetHandler::~GetHandler() {}

	// in GET, there should be a file name in the path
	void GetHandler::handle(const Request& request, Response& response) {
		const std::string& filePath = request.getUri().path;

		if (m_router.fileExists(filePath)) {   // is this absolute path?
											   // TODO: expand filePath with router.getPath()
											   // TODO: maybe also validate access rights in here
			response.setStatusCode(NOT_FOUND); // and reuse the try/catch http::exception pattern
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
