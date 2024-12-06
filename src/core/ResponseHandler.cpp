#include "core/ResponseHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new ResponseHandler object.
	 */
	ResponseHandler::ResponseHandler() {
	}

	/**
	 * @brief Destroys the ResponseHandler object.
	 */
	ResponseHandler::~ResponseHandler() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other ResponseHandler object to copy.
	 */
	ResponseHandler::ResponseHandler(const ResponseHandler &) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other ResponseHandler object to assign from.
	 * @return A reference to the assigned ResponseHandler object.
	 */
	ResponseHandler &ResponseHandler::operator=(const ResponseHandler &) {
		return *this;
	}

	void ResponseHandler::handle(HandleContext &) {
	}

} /* namespace core */
