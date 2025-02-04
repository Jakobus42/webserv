#include "http/GetHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new GetHandler object.
	 */
	GetHandler::GetHandler(GoodRouter& router)
		: ARequestHandler(router) {}

	/**
	 * @brief Destroys the GetHandler object.
	 */
	GetHandler::~GetHandler() {}

	// in GET, there should be a file name in the path
	void GetHandler::handle(const Request& request, Response& response) {
		// const std::string& filePath = request.getUri().path; // is this already the absolute path?
		try {
			FileType fileType = m_router.checkFileType(request.getUri().safeAbsolutePath);
			if (fileType == _NOT_FOUND) { // TODO: replace with actual file name
				throw http::exception(NOT_FOUND, "GET: File doesn't exist");
			}
			// TODO: implement
			// if (fileType == IS_DIRECTORY) {
			//	// autoindex or look for index.html
			// }

			std::ifstream inFile(request.getUri().safeAbsolutePath.c_str(), std::ios::binary);
			if (!inFile.is_open()) {
				throw http::exception(FORBIDDEN, "GET: File isn't accessible"); // TODO: also happens if path doesn't exist, should be NOT_FOUND in that case
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
