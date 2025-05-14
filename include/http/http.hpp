#pragma once

#include <stdexcept>
#include <string>

namespace http {

	enum StatusCode {
		OK = 200,
		CREATED = 201,
		ACCEPTED = 202,
		NO_CONTENT = 204,
		MOVED_PERMANENTLY = 301,
		FOUND = 302,
		SEE_OTHER = 303,
		NOT_MODIFIED = 304,
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED = 405,
		NOT_ACCEPTABLE = 406,
		REQUEST_TIMEOUT = 408,
		CONFLICT = 409,
		GONE = 410,
		LENGTH_REQUIRED = 411,
		PRECONDITION_FAILED = 412,
		PAYLOAD_TOO_LARGE = 413,
		URI_TOO_LONG = 414,
		UNSUPPORTED_MEDIA_TYPE = 415,
		RANGE_NOT_SATISFIABLE = 416,
		EXPECTATION_FAILED = 417,
		IM_A_TEAPOT = 418,
		MISDIRECTED_REQUEST = 421,
		UPGRADE_REQUIRED = 426,
		INTERNAL_SERVER_ERROR = 500,
		NOT_IMPLEMENTED = 501,
		BAD_GATEWAY = 502,
		SERVICE_UNAVAILABLE = 503,
		GATEWAY_TIMEOUT = 504,
		HTTP_VERSION_NOT_SUPPORTED = 505,
		LOOP_DETECTED = 508
	};

	enum Method {
		GET,
		POST,
		DELETE
	};

	class HttpException : public std::runtime_error {
		public:
			explicit HttpException(StatusCode statusCode);
			explicit HttpException(StatusCode statusCode, const std::string& message);
			virtual ~HttpException() throw();

			StatusCode getStatusCode() const;
			const std::string& getMessage() const;

		private:
			StatusCode m_statusCode;
			std::string m_message;
	};

	const char* statusCodeToMessage(StatusCode statusCode);
	StatusCode numToStatusCode(std::size_t code);
	const char* methodToString(Method method);
	Method stringToMethod(const std::string& method);

} // namespace http
