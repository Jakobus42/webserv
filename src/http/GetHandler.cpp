#include "http/GetHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new GetHandler object.
	 */
	GetHandler::GetHandler(const config::Location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the GetHandler object.
	 */
	GetHandler::~GetHandler() {
	}

	void GetHandler::handle(const Request&, Response&) {
	}

} /* namespace http */
