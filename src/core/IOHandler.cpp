#include "core/IOHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new IOHandler object.
	 */
	IOHandler::IOHandler() {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other IOHandler object to copy.
	 */
	IOHandler::IOHandler(const IOHandler &) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other IOHandler object to assign from.
	 * @return A reference to the assigned IOHandler object.
	 */
	IOHandler &IOHandler::operator=(const IOHandler &) {
		return *this;
	}

	void IOHandler::handle(HandleContext &) {
		std::cout << "IO HANDLER DOING IOOOOO" << std::endl;
	}

} /* namespace core */
