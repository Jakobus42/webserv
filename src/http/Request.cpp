#include "http/Request.hpp"

#include <sstream>

namespace http {

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

	/* Start Line */

	const http::Method& Request::getMethod() const { return m_method; }

	const http::Uri& Request::getUri() const { return m_uri; }

	http::Uri& Request::getUri() { return m_uri; }

	void Request::setMethod(const http::Method& method) { m_method = method; }

	void Request::setUri(const http::Uri& uri) { m_uri = uri; }

	/* Debugging */

	std::string Request::toString() const {
		std::ostringstream ss;
		ss << http::methodToString(m_method) << " " << m_uri.toString() << " " << m_version << '\n'
		   << this->AMessage::toString();
		return ss.str();
	}

} /* namespace http */
