#include "http/StatusMessages.hpp"

namespace http {

	StatusMessages::StatusMessages() {
		m_statusMessages[BAD_REQUEST] = "Bad Request";
		m_statusMessages[UNAUTHORIZED] = "Unauthorized";
		m_statusMessages[FORBIDDEN] = "Forbidden";
		m_statusMessages[NOT_FOUND] = "Not Found";
		m_statusMessages[METHOD_NOT_ALLOWED] = "Method Not Allowed";
		m_statusMessages[NOT_ACCEPTABLE] = "Not Acceptable";
		m_statusMessages[REQUEST_TIMEOUT] = "Request Timeout";
		m_statusMessages[CONFLICT] = "Conflict";
		m_statusMessages[GONE] = "Gone";
		m_statusMessages[LENGTH_REQUIRED] = "Length Required";
		m_statusMessages[PAYLOAD_TOO_LARGE] = "Payload Too Large";
		m_statusMessages[URI_TOO_LONG] = "URI Too Long";
		m_statusMessages[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
		m_statusMessages[RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
		m_statusMessages[EXPECTATION_FAILED] = "Expectation Failed";
		m_statusMessages[IM_A_TEAPOT] = "I'm a teapot";
		m_statusMessages[MISDIRECTED_REQUEST] = "Misdirected Request";
		m_statusMessages[UPGRADE_REQUIRED] = "Upgrade Required";
		m_statusMessages[INTERNAL_SERVER_ERROR] = "Internal Server Error";
		m_statusMessages[NOT_IMPLEMENTED] = "Not Implemented";
		m_statusMessages[BAD_GATEWAY] = "Bad Gateway";
		m_statusMessages[SERVICE_UNAVAILABLE] = "Service Unavailable";
		m_statusMessages[GATEWAY_TIMEOUT] = "Gateway Timeout";
		m_statusMessages[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	}

	/**
	 * @brief Destroys the StatusMessages object.
	 */
	StatusMessages::~StatusMessages() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other StatusMessages object to copy.
	 */
	StatusMessages::StatusMessages(const StatusMessages& other) {
		(void)other;
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other StatusMessages object to assign from.
	 * @return A reference to the assigned StatusMessages object.
	 */
	StatusMessages& StatusMessages::operator=(const StatusMessages& rhs) {
		(void)rhs;
		return *this;
	}

	const t_statusMessages& StatusMessages::getStatusMessages() const {
		return m_statusMessages;
	}

} // namespace http
