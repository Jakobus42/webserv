#include "http/DeleteHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new DeleteHandler object.
	 */
	DeleteHandler::DeleteHandler(const config::t_location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the DeleteHandler object.
	 */
	DeleteHandler::~DeleteHandler() {
	}

	void DeleteHandler::handle(const Request&, Response&) {
	}

} /* namespace http */
