#include "http/DeleteHandler.hpp"

#include "http/Router.hpp"

#include <cstdio>

namespace http {

	/**
	 * @brief Constructs a new DeleteHandler object.
	 */
	DeleteHandler::DeleteHandler(Router& router)
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
			std::string safePath = m_router.getSafePath(request.getUri()); // should be the absolute path to the requested file

			if (!m_router.fileExists(safePath + "/" + "file.dat")) { // TODO: replace with actual file name
				throw http::exception(NOT_FOUND, "DELETE: File doesn't exist");
			}

			if (std::remove(safePath.c_str()) != 0) { // unlink the file
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
