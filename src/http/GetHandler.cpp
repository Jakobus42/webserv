#include "http/GetHandler.hpp"

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
		try {
			const config::Location& location = *request.getLocation();
			FileType fileType = m_router.checkFileType(request.getUri().safeAbsolutePath);
			if (fileType == _NOT_FOUND) { // TODO: replace with actual file name
				throw http::exception(NOT_FOUND, "GET: File doesn't exist");
			}
			std::string filePath = request.getUri().safeAbsolutePath;
			if (fileType == DIRECTORY) {
				if (!location.indexFile.empty()) {
					// TODO: if no index files exist / are valid, fall back on autoindex
					//       if autoindex is then false, return 403 as usual
					for (std::vector<std::string>::const_iterator file = location.indexFile.begin(); file != location.indexFile.end(); ++file) {
						std::string tmpFilePath = filePath + "/" + *file;
						FileType tmpFileType = m_router.checkFileType(tmpFilePath);
						if (tmpFileType == FILE) {
							filePath = tmpFilePath;
							break;
						}
					}
				} else if (location.autoindex == true) { // TODO: should also happen if location.indexFile isn't empty, but no indexFile is found at the location
														 // return autoindex
				} else {
					throw http::exception(FORBIDDEN, "Requested location does not have an index");
				}
			}
			std::ifstream inFile(filePath.c_str(), std::ios::binary);
			if (!inFile.is_open()) {
				throw http::exception(FORBIDDEN, "GET: File could not be opened"); // TODO: also happens if path doesn't exist, should be NOT_FOUND in that case
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
