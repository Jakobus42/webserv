#include "http2/Response.hpp"

namespace http2 {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: AMessage() {
	}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Response object to copy.
	 */
	Response::Response(const Response&)
		: AMessage() {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Response object to assign from.
	 * @return A reference to the assigned Response object.
	 */
	Response& Response::operator=(const Response&) {
		return *this;
	}

} /* namespace http2 */
