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

	bool GetHandler::getFilePath(const config::Location& location, const std::string& filePath, std::string& target) {
		if (!location.indexFile.empty()) {
			for (std::vector<std::string>::const_iterator file = location.indexFile.begin(); file != location.indexFile.end(); ++file) {
				std::string tmpFilePath = filePath + "/" + *file;
				FileType tmpFileType = m_router.checkFileType(tmpFilePath);
				if (tmpFileType == FILE) { // return path to first file in index that exists and isn't a directory
					std::cout << "getFilePath found file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
					target.assign(tmpFilePath);
					return true;
				}
			}
		}
		return false;
	}

	// in GET, there should be a file name in the path
	void GetHandler::handle(const Request& request, Response& response) {
		try {
			const config::Location& location = *request.getLocation();
			FileType fileType = m_router.checkFileType(request.getUri().safeAbsolutePath);
			if (fileType == _NOT_FOUND) {
				throw http::exception(NOT_FOUND, "GET: File doesn't exist");
			}
			std::string filePath = "";
			std::string body = "";
			if (fileType == FILE) {
				filePath = request.getUri().safeAbsolutePath;
			}
			if (fileType == DIRECTORY) {
				if (!location.indexFile.empty()) {
					getFilePath(location, request.getUri().safeAbsolutePath, filePath); // returns true only if filePath isn't still ""
				}
				if (filePath.empty() && location.autoindex == true) { // TODO: pretty sus, verify that this works
					if (getDirectoryListing(filePath, body, Router::joinPath(m_router.getGlobalRoot().root)) == false) {
						throw http::exception(INTERNAL_SERVER_ERROR, "getDirectoryListing broke during autoindex");
					}
					// we now have HTML body for autoindex
				} else {
					throw http::exception(FORBIDDEN, "GET: Requested location does not have an index");
				}
			}
			std::stringstream buffer;
			if (filePath.empty()) {
				response.setBody(body);
			} else {
				std::ifstream inFile(filePath.c_str(), std::ios::binary);
				if (!inFile.is_open()) {
					throw http::exception(FORBIDDEN, "GET: File could not be opened"); // TODO: also happens if path doesn't exist, should be NOT_FOUND in that case
				}
				buffer << inFile.rdbuf();
				response.setBody(buffer.str());
			}
			response.setHeader("Content-Length", shared::string::to_string(response.getBody().size()));
			response.setStatusCode(OK);
		} catch (const http::exception& e) {
			std::cout << "CRAP, " << e.getMessage() << "; " << e.getCode() << std::endl;
			response.setStatusCode(e.getCode());
			return handleError(response);
		}
	}

} /* namespace http */
