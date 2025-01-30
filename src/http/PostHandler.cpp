#include "http/PostHandler.hpp"

#include <fcntl.h>
#include <unistd.h>

namespace http {

	/**
	 * @brief Constructs a new PostHandler object.
	 */
	PostHandler::PostHandler(const config::Location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the PostHandler object.
	 */
	PostHandler::~PostHandler() {
	}

	std::string PostHandler::getFilePath(const std::string& path) {
		(void)path;
		return "/foo/bar/baz";
	}

	void PostHandler::handle(const Request& request, Response& response) {
		(void)request;
		(void)response;
		// 1. Check if location allows uploads
		// if (!m_location.hasPostAndExists) {
		// 	response.setStatusCode(FORBIDDEN);
		// 	return;
		// }

		// 2. Validate upload path exists and is writable
		// std::string uploadPath = "";
		// try {
		// 	uploadPath = request.getUri().path; // TODO: shouldn't contain file name, path probably does
		// } catch (...) {
		// 	response.setStatusCode(BAD_REQUEST);
		// 	return handleError(request, response);
		// }
		// std::cout << "Upload path is '" << uploadPath << "'" << std::endl;
		// if (access(uploadPath.c_str(), W_OK) != 0) { // because the file doesn't exist yet - would always be inaccessible
		// 	response.setStatusCode(FORBIDDEN);
		// 	std::cout << "File inaccessible, setting status code to FORBIDDEN" << std::endl;
		// 	return handleError(request, response);
		// }

		// std::cout << "File accessible" << std::endl;

		// // body should already be there & checked for validity

		// try {
		// 	std::string filename = "lol";

		// 	std::string fullPath = uploadPath + "/" + filename;
		// 	std::ofstream outFile(fullPath.c_str(), std::ios::binary);
		// 	if (!outFile) {
		// 		response.setStatusCode(INTERNAL_SERVER_ERROR);
		// 		return handleError(request, response);
		// 	}

		// 	outFile.write(response.getBody().c_str(), response.getBody().length());
		// 	outFile.close();

		// 	// 9. Set success response
		// 	response.setStatusCode(CREATED);
		// 	response.setHeader("Location", request.getUri().path + "/" + filename);

		// } catch (const std::exception& e) {
		// 	response.setStatusCode(INTERNAL_SERVER_ERROR);
		// 	return handleError(request, response);
		// }
	}

} /* namespace http */
