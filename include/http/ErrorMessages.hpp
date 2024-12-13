#pragma once

#include "shared/defines.hpp"

#include <map>
#include <string>

namespace http {

	typedef std::pair<http::StatusCode, std::string> t_errorMessage;
	typedef std::map<t_errorMessage::first_type, t_errorMessage::second_type> t_errorMessages;

	/**
	 * @class ErrorMessages
	 * @brief Singleton class to store and retrieve error messages
	 */
	class ErrorMessages {
		public:
			~ErrorMessages();

			static const ErrorMessages &getInstance() {
				static ErrorMessages instance;
				return instance;
			}

			// singleton is otherwise lazily initialized
			static void eagerInit() {
				(void)getInstance();
			}

			const t_errorMessages &getErrorMessages() const {
				return m_errorMessages;
			}
		private:
			ErrorMessages() {
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

			ErrorMessages(const ErrorMessages &other);
			ErrorMessages &operator=(const ErrorMessages &rhs);

			t_errorMessages m_errorMessages;
	};

} /* namespace http */
