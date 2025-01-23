#include "http/PostHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new PostHandler object.
	 */
	PostHandler::PostHandler(const config::t_location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the PostHandler object.
	 */
	PostHandler::~PostHandler() {
	}

	void PostHandler::handle(const Request&, Response&) {
	}

} /* namespace http */
