#include "http/Response.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: m_rawResponse("")
		, m_statusCode(UNKNWN)
		, m_status(PENDING_WRITE) {}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Response object to copy.
	 */
	Response::Response(const Response& other)
		: m_rawResponse(other.getRawResponse())
		, m_statusCode(other.getStatusCode())
		, m_status(other.getStatus()) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Response object to assign from.
	 * @return A reference to the assigned Response object.
	 */
	Response& Response::operator=(const Response& rhs) {
		if (this == &rhs)
			return *this;
		m_rawResponse = rhs.getRawResponse();
		m_status = rhs.getStatus();
		m_statusCode = rhs.getStatusCode();
		return *this;
	}

	const std::string& Response::getRawResponse(void) const {
		return m_rawResponse;
	}

	ResponseBuilderStatus Response::getStatus(void) const {
		return m_status;
	}

	StatusCode Response::getStatusCode(void) const {
		return m_statusCode;
	}

	void Response::setRawResponse(const std::string& to) {
		m_rawResponse = to;
	}

	bool Response::done(void) const {
		return true;
	}

	void Response::reset(void) {
		m_rawResponse = "";
		m_status = PENDING_WRITE;
		m_statusCode = UNKNWN;
	}

} /* namespace http */
