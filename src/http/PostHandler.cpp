#include "http/PostHandler.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "http/Router.hpp"

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

	// join uri.path and fileName
	std::string PostHandler::getFilePath(const std::string& path) {
		(void)path;
		return "/foo/bar/baz";
	}

	void PostHandler::handle(const Request& request, Response& response) {
		const config::Location& location = Router::getLocation(request.getUri()); // TODO: implement
		if (!location.acceptsFileUpload()) {
			response.setStatusCode(FORBIDDEN);
			return handleError(request, response);
		}

		std::string uploadDir = location.root;
		std::string filename = "uploaded.dat";
		std::string fullPath = uploadDir + "/" + filename;

		std::ofstream outFile(fullPath.c_str(), std::ios::binary);
		if (!outFile.is_open()) {
			response.setStatusCode(FORBIDDEN);
			return handleError(request, response);
		}
		outFile.write(request.getBody().data(), request.getBody().size());
		outFile.close();

		response.setStatusCode(CREATED);
		response.setHeader("Content-Length", "0");

		// 	uploadPath = request.getUri().path; // TODO: shouldn't contain file name, path probably does
		// body should already be there & checked for validity
		// TODO: ensure
	}

} /* namespace http */
