#include "http2/Request.hpp"

namespace http2 {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: AMessage()
		, m_method(http::GET)
		, m_uri() {
	}

	/**
	 * @brief Destroys the Request object.
	 */
	Request::~Request() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Request object to copy.
	 */
	Request::Request(const Request&)
		: AMessage() {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Request object to assign from.
	 * @return A reference to the assigned Request object.
	 */
	Request& Request::operator=(const Request&) {
		return *this;
	}

	/* Start Line */

	const http::Method& Request::getMethod() const { return m_method; }

	const http2::Uri& Request::getUri() const { return m_uri; }

	void Request::setMethod(const http::Method& method) { m_method = method; }

	void Request::setUri(const http2::Uri& uri) { m_uri = uri; }

	/* Debugging */

	std::string Request::toString() const {
		return http::getMethodString(m_method) + " " + m_uri.toString() + " " + m_version + this->AMessage::toString();
	}

} /* namespace http2 */
