#include "http/Response.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: m_rawResponse("") {}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Response object to copy.
	 */
	Response::Response(const Response& other) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Response object to assign from.
	 * @return A reference to the assigned Response object.
	 */
	Response& Response::operator=(const Response& rhs) {
		if (this == &rhs)
			return *this;
		m_rawResponse = rhs.getRawResponse();
		return *this;
	}

	const std::string& Response::getRawResponse(void) const {
		return m_rawResponse;
	}

	StatusCode Response::getStatusCode(void) const {
		return m_statusCode;
	}

} /* namespace http */
