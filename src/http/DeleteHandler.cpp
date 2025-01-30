#include "http/DeleteHandler.hpp"

#include "http/Router.hpp"

#include <cstdio>

namespace http {

	/**
	 * @brief Constructs a new DeleteHandler object.
	 */
	DeleteHandler::DeleteHandler(const config::Location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the DeleteHandler object.
	 */
	DeleteHandler::~DeleteHandler() {
	}

	void DeleteHandler::handle(const Request& request, Response& response) {
		const std::string& filePath = request.getUri().path;

		if (!Router::fileExists(filePath)) {   // if (fileCantBeFound() || fileCantBeDeleted())
			response.setStatusCode(NOT_FOUND); // currently hard coded
			return handleError(request, response);
		} else {
			if (std::remove(filePath.c_str()) != 0) { // unlink the file
				response.setStatusCode(FORBIDDEN);
				return handleError(request, response);
			}
			// either return 204 No Content when there is no body
			// or return 200 OK if there's information in the body
			// i.e. a html page saying "file /foo/bar.baz was deleted"
			response.setStatusCode(NO_CONTENT);
			response.setHeader("Content-Length", "0");
		}
	}

} /* namespace http */
