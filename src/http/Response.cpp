#include "http/Response.hpp"

#include "http/Request.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: m_rawResponse("")
		, m_statusCode(UNKNWN)
		, m_statusLine("")
		, m_headerFields()
		, m_body()
		, m_builderStatus(PENDING_WRITE) {}

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
		, m_statusLine(other.getStatusLine())
		, m_headerFields(other.getHeaderFields())
		, m_body(other.getBody())
		, m_builderStatus(other.getBuilderStatus()) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Response object to assign from.
	 * @return A reference to the assigned Response object.
	 */
	Response& Response::operator=(const Response& rhs) {
		if (this == &rhs)
			return *this;
		m_rawResponse = rhs.getRawResponse();
		m_statusCode = rhs.getStatusCode();
		m_statusLine = rhs.getStatusLine();
		m_headerFields = rhs.getHeaderFields();
		m_body = rhs.getBody();
		m_builderStatus = rhs.getBuilderStatus();
		return *this;
	}

	ResponseBuilderStatus Response::getBuilderStatus(void) const {
		return m_builderStatus;
	}

	const std::string& Response::getRawResponse(void) const {
		return m_rawResponse;
	}

	StatusCode Response::getStatusCode(void) const {
		return m_statusCode;
	}

	const std::string& Response::getStatusLine(void) const {
		return m_statusLine;
	}

	const t_headerFields& Response::getHeaderFields(void) const {
		return m_headerFields;
	}

	const std::string& Response::getBody(void) const {
		return m_body;
	}

	void Response::setRawResponse(const std::string& to) {
		m_rawResponse = to;
	}

	bool Response::done(void) const {
		return true;
	}

	void Response::buildFromRequest(const Request& request) {
		(void)request;
	}

	void Response::reset(void) {
		m_rawResponse = "";
		m_builderStatus = PENDING_WRITE;
		m_statusCode = UNKNWN;
	}

} /* namespace http */
