#include "http/DeleteHandler.hpp"

#include <cstdio>

namespace http {

	/**
	 * @brief Constructs a new DeleteHandler object.
	 */
	DeleteHandler::DeleteHandler(GoodRouter& router)
		: ARequestHandler(router) {
	}

	/**
	 * @brief Destroys the DeleteHandler object.
	 */
	DeleteHandler::~DeleteHandler() {
	}

	// in GET, there should be a file name in the path
	// on success: either return 204 No Content when there is no body
	// or return 200 OK if there's information in the body
	// i.e. a html page saying "file /foo/bar.baz was deleted"
	void DeleteHandler::handle(const Request& request, Response& response) {
		try {
			const config::Location& location = *request.getLocation();
			FileType fileType = m_router.checkFileType(request.getUri().safeAbsolutePath);
			if (fileType == _NOT_FOUND) { // TODO: replace with actual file name
				throw http::exception(NOT_FOUND, "DELETE: File doesn't exist");
			}
			if (fileType == DIRECTORY) {
				throw http::exception(FORBIDDEN, "DELETE: Expected file; received directory");
			}
			if (std::remove(request.getUri().safeAbsolutePath.c_str()) != 0) { // unlink the file
				throw http::exception(FORBIDDEN, "DELETE: File could not be removed");
			}
			response.setStatusCode(NO_CONTENT);
			response.setHeader("Content-Length", "0");
		} catch (const http::exception& e) {
			std::cout << "FUCK, " << e.getMessage() << std::endl;
			response.setStatusCode(e.getCode());
			return handleError(response);
		}
	}

} /* namespace http */
