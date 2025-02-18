#include "http/PostHandler.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "shared/fileUtils.hpp"

namespace http {

	/**
	 * @brief Constructs a new PostHandler object.
	 */
	PostHandler::PostHandler(Router& router)
		: ARequestHandler(router) {}

	/**
	 * @brief Destroys the PostHandler object.
	 */
	PostHandler::~PostHandler() {}

	// in POST, there should be no file name in the path
	void PostHandler::handle(const Request& request, Response& response) {
		try {
			FileType fileType = m_router.checkFileType(request.getUri().safeAbsolutePath);
			if (fileType == _NOT_FOUND) {
				throw http::exception(FORBIDDEN, "POST: Directory could not be found");
			}
			if (fileType == FILE) {
				throw http::exception(FORBIDDEN, "POST: Expected directory; received file");
			}
			if (!request.getLocation()->acceptsFileUpload()) {
				throw http::exception(FORBIDDEN, "Location does not accept file uploads (no POST)");
			}
			std::string fileName = "uploaded.dat";
			if (request.hasHeader("x-cool-filename")) {
				const std::vector<std::string>& filenameHeader = request.getHeader("x-cool-filename");
				if (!filenameHeader.empty() && !filenameHeader.at(0).empty()) {
					std::cout << "Filename header: " << filenameHeader.at(0) << std::endl;
					fileName = filenameHeader.at(0);
				} else {
					std::cout << "Filename empty! Falling back to default..." << std::endl;
				}
			}
			std::string absoluteFilePath = request.getUri().safeAbsolutePath + "/" + fileName; // TODO: replace with actual file name
			std::cout << "Checking path for posted file: " << absoluteFilePath << std::endl;
			if (shared::file::fileExists(absoluteFilePath)) {
				throw http::exception(CONFLICT, "POST: A file with this name already exists");
			}
			std::cout << "Saving POSTed file as " << absoluteFilePath << std::endl;
			std::ofstream outFile(absoluteFilePath.c_str(), std::ios::binary);
			if (!outFile.is_open()) {
				throw http::exception(FORBIDDEN, "POST: File couldn't be opened");
			}
			outFile.write(request.getBody().data(), request.getBody().size()); // TODO: verify whether this could break & if so, what error to throw
			outFile.close();
			response.setStatusCode(CREATED);
			response.setHeader("Content-Length", "0");
		} catch (const http::exception& e) {
			std::cout << "DANG, " << e.getMessage() << std::endl;
			response.setStatusCode(e.getStatusCode());
			return handleError(response);
		}
	}

} /* namespace http */
