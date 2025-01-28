#include "http/http.hpp"

#include <cstdio>

namespace http {

	const std::string& getStatusMessage(StatusCode statusCode) {
		static std::map<StatusCode, std::string> statusMessages;
		if (statusMessages.empty()) {
			statusMessages[UNKNWN] = "Unknown Status Code, Whoopsie";
			statusMessages[OK] = "OK";
			statusMessages[CREATED] = "Created";
			statusMessages[ACCEPTED] = "Accepted";
			statusMessages[NO_CONTENT] = "No Content";
			statusMessages[MOVED_PERMANENTLY] = "Moved Permanently";
			statusMessages[FOUND] = "Found";
			statusMessages[SEE_OTHER] = "See Other";
			statusMessages[NOT_MODIFIED] = "Not Modified";
			statusMessages[BAD_REQUEST] = "Bad Request";
			statusMessages[UNAUTHORIZED] = "Unauthorized";
			statusMessages[FORBIDDEN] = "Forbidden";
			statusMessages[NOT_FOUND] = "Not Found";
			statusMessages[METHOD_NOT_ALLOWED] = "Method Not Allowed";
			statusMessages[NOT_ACCEPTABLE] = "Not Acceptable";
			statusMessages[REQUEST_TIMEOUT] = "Request Timeout";
			statusMessages[CONFLICT] = "Conflict";
			statusMessages[GONE] = "Gone";
			statusMessages[LENGTH_REQUIRED] = "Length Required";
			statusMessages[PRECONDITION_FAILED] = "Precondition Failed";
			statusMessages[PAYLOAD_TOO_LARGE] = "Payload Too Large";
			statusMessages[URI_TOO_LONG] = "URI Too Long";
			statusMessages[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
			statusMessages[RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
			statusMessages[EXPECTATION_FAILED] = "Expectation Failed";
			statusMessages[IM_A_TEAPOT] = "I'm a teapot";
			statusMessages[MISDIRECTED_REQUEST] = "Misdirected Request";
			statusMessages[UPGRADE_REQUIRED] = "Upgrade Required";
			statusMessages[INTERNAL_SERVER_ERROR] = "Internal Server Error";
			statusMessages[NOT_IMPLEMENTED] = "Not Implemented";
			statusMessages[BAD_GATEWAY] = "Bad Gateway";
			statusMessages[SERVICE_UNAVAILABLE] = "Service Unavailable";
			statusMessages[GATEWAY_TIMEOUT] = "Gateway Timeout";
			statusMessages[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
		}

		std::map<StatusCode, std::string>::const_iterator it = statusMessages.find(statusCode);
		if (it != statusMessages.end()) {
			return it->second;
		}

		static const std::string unknownStatus = "Unknown Status Code, Whoopsie";
		return unknownStatus;
	}

	std::string getMethodString(Method method) {
		switch (method) {
			case GET:
				return "GET";
			case POST:
				return "POST";
			case DELETE:
				return "DELETE";
		}
		throw http::exception(NOT_IMPLEMENTED, "method not implemented");
	}

	Method stringToMethod(const std::string& method) {
		if (method == "GET") {
			return GET;
		} else if (method == "POST") {
			return POST;
		} else if (method == "DELETE") {
			return DELETE;
		}
		throw http::exception(NOT_IMPLEMENTED, "method not implemented");
	}

	std::string getErrorPage(StatusCode statusCode) {
		static const char* errorPageTemplate =
			"<!DOCTYPE html><html lang=\"en\"><head><title>%d %s</title><style>"
			"body{background-color:#2b3042;justify-content: center;text-align: center;color:#d3dbeb;}"
			"h1{font-size:5rem;}p{font-size: 1.5rem;padding-bottom: 10px;}a{"
			"text-decoration: none;color: #d3dbeb;padding: 10px;border: 3px solid #d3dbeb;font-weight: bold;}</style>"
			"</head><body><h1>%d</h1><p>%s</p><a href=\"/home_directory\">Go Back to Home</a></body></html>";

		char errorPage[1024];
		const char* statusMessage = getStatusMessage(statusCode).c_str();
		snprintf(errorPage, sizeof(errorPage), errorPageTemplate, statusCode, statusMessage, statusCode, statusMessage);

		return std::string(errorPage);
	}

	exception::exception(StatusCode statusCode)
		: std::runtime_error(buildErrorMessage(statusCode, getStatusMessage(statusCode)))
		, m_statusCode(statusCode)
		, m_message(getStatusMessage(statusCode)) {
	}

	exception::exception(StatusCode statusCode, const std::string& message)
		: std::runtime_error(buildErrorMessage(statusCode, message))
		, m_statusCode(statusCode)
		, m_message(message) {
	}

	exception::~exception() throw() {}

	StatusCode exception::getCode() const {
		return m_statusCode;
	}

	const std::string& exception::getMessage() const {
		return m_message;
	}

	std::string exception::buildErrorMessage(StatusCode statusCode, const std::string& message) {
		std::ostringstream oss;
		oss << static_cast<int>(statusCode) << " - " << message;
		return oss.str(); // Only one construction of the string
	}

} // namespace http