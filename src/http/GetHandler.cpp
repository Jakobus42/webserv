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
		// const std::string& filePath = request.getUri().path; // is this already the absolute path?
		try {
			std::string safePath = m_router.getSafePath(request.getUri()); // should be the absolute path to the requested file

			if (!m_router.fileExists(safePath)) { // TODO: replace with actual file name
				throw http::exception(NOT_FOUND, "GET: File doesn't exist");
			}

			std::ifstream inFile(safePath.c_str(), std::ios::binary);
			if (!inFile.is_open()) {
				throw http::exception(FORBIDDEN, "GET: File isn't accessible");
			}

			std::stringstream buffer;
			buffer << inFile.rdbuf();
			response.setBody(buffer.str());
			response.setHeader("Content-Length", shared::string::to_string(buffer.str().size()));
			response.setStatusCode(OK);
		} catch (const http::exception& e) {
			std::cout << "CRAP, " << e.getMessage() << "; " << e.getCode() << std::endl;
			response.setStatusCode(e.getCode());
			return handleError(response);
		}
	}

} /* namespace http */
