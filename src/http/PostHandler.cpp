#include "http/PostHandler.hpp"

#include <fcntl.h>
#include <unistd.h>

#include "http/Router.hpp"

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
			// const config::Location& location = m_router.getLocation(request.getUri()); // TODO: implement
			// location should probably be called deepestMatchingLocation
			// we somehow need to both get:
			// - deepest matching Location & its absolute root path
			// - subdirectory path
			// -> concatenate the two to get the overall absolute target path (including fileName)
			// if (!location.acceptsFileUpload()) {
			// 	throw http::exception(FORBIDDEN, "Location does not accept file uploads (no POST)");
			// }

			std::string safePath = m_router.getSafePath(request.getUri());	// should only return the path to the directory
																			// we then concatenate on the file name
			std::string absoluteFilePath = safePath + "/" + "uploaded.dat"; // TODO: replace with actual file name
			std::ofstream outFile(absoluteFilePath.c_str(), std::ios::binary);
			if (!outFile.is_open()) {
				throw http::exception(FORBIDDEN, "POST: File couldn't be opened");
			}
			outFile.write(request.getBody().data(), request.getBody().size()); // TODO: verify whether this could break & if so, what error to throw
			outFile.close();
			response.setStatusCode(CREATED);
			response.setHeader("Content-Length", "0");
		} catch (const http::exception& e) {
			response.setStatusCode(e.getCode());
			return handleError(request, response);
		}
		// 	uploadPath = request.getUri().path; // TODO: shouldn't contain file name, path probably does
		// body should already be there & checked for validity
		// TODO: ensure
	}

} /* namespace http */
