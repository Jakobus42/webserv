#include "http/ErrorMessages.hpp"

namespace http {

	/**
	 * @brief Destroys the ErrorMessages object.
	 */
	ErrorMessages::~ErrorMessages() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other ErrorMessages object to copy.
	 */
	ErrorMessages::ErrorMessages(const ErrorMessages &other) {
		(void)other;
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other ErrorMessages object to assign from.
	 * @return A reference to the assigned ErrorMessages object.
	 */
	ErrorMessages &ErrorMessages::operator=(const ErrorMessages &rhs) {
		(void)rhs;
		return *this;
	}

} // namespace http
