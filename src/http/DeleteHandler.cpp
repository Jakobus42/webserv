#include "http/DeleteHandler.hpp"

#include <cstdio>

namespace http {

	/**
	 * @brief Constructs a new DeleteHandler object.
	 */
	DeleteHandler::DeleteHandler(Router& router)
		: ARequestHandler(router) {}

	/**
	 * @brief Destroys the DeleteHandler object.
	 */
	DeleteHandler::~DeleteHandler() {}

	void DeleteHandler::deleteFile(const Request& request, Response& response, const std::string& filePath) {
		FileType fileType = request.getFileType();

		if (fileType == _NOT_FOUND) { // TODO: replace with actual file name
			throw http::exception(NOT_FOUND, "DELETE: File doesn't exist");
		}
		if (fileType == DIRECTORY) {
			throw http::exception(FORBIDDEN, "DELETE: Expected file; received directory");
		}
		if (std::remove(filePath.c_str()) != 0) {
			throw http::exception(FORBIDDEN, "DELETE: File could not be removed");
		}

		response.setStatusCode(NO_CONTENT);
		response.setHeader("Content-Length", "0");
		m_state = DONE;
	}

	// in GET, there should be a file name in the path
	// on success: either return 204 No Content when there is no body
	// or return 200 OK if there's information in the body
	// i.e. a html page saying "file /foo/bar.baz was deleted"
	void DeleteHandler::handle(const Request& request, Response& response) {
		switch (m_state) {
			case PENDING:
			case PROCESSING: {
				return deleteFile(request, response, request.getUri().safeAbsolutePath);
			}

			case DONE: {
				return;
			}

			default:
				throw http::exception(INTERNAL_SERVER_ERROR, "DeleteHandler: Invalid Handler State");
		}
	}

} /* namespace http */
