#include "http/DeleteHandler.hpp"

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
		if (false) {								   // if (fileCantBeFound() || fileCantBeDeleted())
			response.setBody(getErrorPage(NOT_FOUND)); // or FORBIDDEN, or maybe something else...
			return handleError(request, response);
		} else {
			std::remove(request.getUri().path.c_str()); // unlink the file
			// either return 204 No Content when there is no body
			// or return 200 OK if there's information in the body
			// i.e. a html page saying "file /foo/bar.baz was deleted"
			response.setStatusCode(OK);
		}
	}

} /* namespace http */
