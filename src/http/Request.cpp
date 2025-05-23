#include "http/Request.hpp"

#include <sstream>

namespace http {

	Request::Request()
		: AMessage()
		, m_method(GET)
		, m_uri()
		, m_type(FETCH) {
	}

	Request::~Request() {
	}

	/* Start Line */

	const Method& Request::getMethod() const { return m_method; }

	const Uri& Request::getUri() const { return m_uri; }

	Uri& Request::getUri() { return m_uri; }

	void Request::setMethod(const Method& method) { m_method = method; }

	void Request::setUri(const Uri& uri) { m_uri = uri; }

	/* Helpers */

	void Request::setType(Type type) { m_type = type; }

	Request::Type Request::getType() const { return m_type; }

	/* Debugging */

	std::string Request::toString() const {
		std::ostringstream ss;
		ss << methodToString(m_method) << " " << m_uri.toString() << " " << m_version << '\n'
		   << this->AMessage::toString();
		return ss.str();
	}

} /* namespace http */
