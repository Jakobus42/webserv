#include "http/RequestParser.hpp"

namespace http {

	/**
	 * @brief Constructs a new RequestParser object.
	 */
	RequestParser::RequestParser() {
	}

	/**
	 * @brief Destroys the RequestParser object.
	 */
	RequestParser::~RequestParser() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other RequestParser object to copy.
	 */
	RequestParser::RequestParser(const RequestParser&) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other RequestParser object to assign from.
	 * @return A reference to the assigned RequestParser object.
	 */
	RequestParser& RequestParser::operator=(const RequestParser&) {
		return *this;
	}

} /* namespace http */
