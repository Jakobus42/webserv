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

	void GetHandler::getFilePath(const config::Location& location, const std::string& filePath, std::string& target) {
		for (std::vector<std::string>::const_iterator fileName = location.indexFile.begin(); fileName != location.indexFile.end(); ++fileName) {
			std::string tmpFilePath = filePath + "/" + *fileName;
			FileType tmpFileType = m_router.checkFileType(tmpFilePath);
			if (tmpFileType == FILE) { // return path to first file in index that exists and isn't a directory
				std::cout << "getFilePath found file!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
				target.assign(tmpFilePath);
				return;
			}
		}
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
			std::string autoindexBody = "";
			if (fileType == FILE) {
				filePath = request.getUri().safeAbsolutePath;
			}
			if (fileType == DIRECTORY) {
				if (!location.indexFile.empty()) {
					getFilePath(location, request.getUri().safeAbsolutePath, filePath); // returns true only if filePath isn't still ""
				}
				if (filePath.empty()) {
					if (location.autoindex == true) { // TODO: pretty sus, verify that this works
						autoindexBody = getDirectoryListing(request.getUri(), location, shared::string::joinPath(m_router.getGlobalRoot().rootAsTokens));
					} else {
						throw http::exception(FORBIDDEN, "GET: Requested location does not have an index");
					}
				}
			}
			// if we didn't throw, we now have filePath, or filePath is empty and we have the HTML body for autoindex
			std::stringstream buffer;
			if (filePath.empty()) {
				response.setBody(autoindexBody);
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
			std::cout << "CRAP, " << e.getMessage() << "; " << e.getStatusCode() << std::endl;
			response.setStatusCode(e.getStatusCode());
			return handleError(response);
		}
	}

} /* namespace http */
