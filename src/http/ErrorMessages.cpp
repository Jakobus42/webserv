#include "http/ErrorMessages.hpp"

namespace http {

	ErrorMessages::ErrorMessages() {
		m_errorMessages[BAD_REQUEST] = "Bad Request";
		m_errorMessages[UNAUTHORIZED] = "Unauthorized";
		m_errorMessages[FORBIDDEN] = "Forbidden";
		m_errorMessages[NOT_FOUND] = "Not Found";
		m_errorMessages[METHOD_NOT_ALLOWED] = "Method Not Allowed";
		m_errorMessages[NOT_ACCEPTABLE] = "Not Acceptable";
		m_errorMessages[REQUEST_TIMEOUT] = "Request Timeout";
		m_errorMessages[CONFLICT] = "Conflict";
		m_errorMessages[GONE] = "Gone";
		m_errorMessages[LENGTH_REQUIRED] = "Length Required";
		m_errorMessages[PAYLOAD_TOO_LARGE] = "Payload Too Large";
		m_errorMessages[URI_TOO_LONG] = "URI Too Long";
		m_errorMessages[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
		m_errorMessages[RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
		m_errorMessages[EXPECTATION_FAILED] = "Expectation Failed";
		m_errorMessages[IM_A_TEAPOT] = "I'm a teapot";
		m_errorMessages[MISDIRECTED_REQUEST] = "Misdirected Request";
		m_errorMessages[UPGRADE_REQUIRED] = "Upgrade Required";
		m_errorMessages[INTERNAL_SERVER_ERROR] = "Internal Server Error";
		m_errorMessages[NOT_IMPLEMENTED] = "Not Implemented";
		m_errorMessages[BAD_GATEWAY] = "Bad Gateway";
		m_errorMessages[SERVICE_UNAVAILABLE] = "Service Unavailable";
		m_errorMessages[GATEWAY_TIMEOUT] = "Gateway Timeout";
		m_errorMessages[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
	}

	/**
	 * @brief Destroys the ErrorMessages object.
	 */
	ErrorMessages::~ErrorMessages() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other ErrorMessages object to copy.
	 */
	ErrorMessages::ErrorMessages(const ErrorMessages& other) {
		(void)other;
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other ErrorMessages object to assign from.
	 * @return A reference to the assigned ErrorMessages object.
	 */
	ErrorMessages& ErrorMessages::operator=(const ErrorMessages& rhs) {
		(void)rhs;
		return *this;
	}

	const t_errorMessages& ErrorMessages::getErrorMessages() const {
		return m_errorMessages;
	}

} // namespace http
