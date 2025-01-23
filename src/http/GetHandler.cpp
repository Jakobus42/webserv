#include "http/GetHandler.hpp"

namespace http {

	/**
	 * @brief Constructs a new GetHandler object.
	 */
	GetHandler::GetHandler(const config::t_location& locations)
		: ARequestHandler(locations) {
	}

	/**
	 * @brief Destroys the GetHandler object.
	 */
	GetHandler::~GetHandler() {
	}

	void GetHandler::handle(const Request& req, Response& res) {
		std::string path = req.getUri(); // todo: to the guy who does uri stuff replace this with the path lol
	}

} /* namespace http */
