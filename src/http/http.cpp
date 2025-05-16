#include "http/http.hpp"

#include "shared/stringUtils.hpp"

namespace http {

	HttpException::HttpException(StatusCode statusCode)
		: std::runtime_error(shared::string::toString(static_cast<int>(statusCode)) + " - " + std::string(statusCodeToMessage(statusCode)))
		, m_statusCode(statusCode)
		, m_message(statusCodeToMessage(statusCode)) {
	}

	HttpException::HttpException(StatusCode statusCode, const std::string& message)
		: std::runtime_error(shared::string::toString(static_cast<int>(statusCode)) + " - " + message)
		, m_statusCode(statusCode)
		, m_message(message) {
	}

	HttpException::~HttpException() throw() {}

	StatusCode HttpException::getStatusCode() const { return m_statusCode; }

	const std::string& HttpException::getMessage() const { return m_message; }

	const char* statusCodeToMessage(StatusCode statusCode) {
		switch (statusCode) {
			case OK: return "OK";
			case CREATED: return "Created";
			case ACCEPTED: return "Accepted";
			case NO_CONTENT: return "No Content";
			case MOVED_PERMANENTLY: return "Moved Permanently";
			case FOUND: return "Found";
			case SEE_OTHER: return "See Other";
			case NOT_MODIFIED: return "Not Modified";
			case BAD_REQUEST: return "Bad Request";
			case UNAUTHORIZED: return "Unauthorized";
			case FORBIDDEN: return "Forbidden";
			case NOT_FOUND: return "Not Found";
			case METHOD_NOT_ALLOWED: return "Method Not Allowed";
			case NOT_ACCEPTABLE: return "Not Acceptable";
			case REQUEST_TIMEOUT: return "Request Timeout";
			case CONFLICT: return "Conflict";
			case GONE: return "Gone";
			case LENGTH_REQUIRED: return "Length Required";
			case PRECONDITION_FAILED: return "Precondition Failed";
			case PAYLOAD_TOO_LARGE: return "Payload Too Large";
			case URI_TOO_LONG: return "URI Too Long";
			case UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
			case RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
			case EXPECTATION_FAILED: return "Expectation Failed";
			case IM_A_TEAPOT: return "I'm a teapot";
			case MISDIRECTED_REQUEST: return "Misdirected Request";
			case UPGRADE_REQUIRED: return "Upgrade Required";
			case INTERNAL_SERVER_ERROR: return "Internal Server Error";
			case NOT_IMPLEMENTED: return "Not Implemented";
			case BAD_GATEWAY: return "Bad Gateway";
			case SERVICE_UNAVAILABLE: return "Service Unavailable";
			case GATEWAY_TIMEOUT: return "Gateway Timeout";
			case HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
			case LOOP_DETECTED: return "Loop Detected";
			default: throw HttpException(INTERNAL_SERVER_ERROR, "Invalid status code");
		}
		throw HttpException(INTERNAL_SERVER_ERROR, "Invalid status code");
	}

	StatusCode numToStatusCode(std::size_t code) {
		switch (code) {
			case 200: return OK;
			case 201: return CREATED;
			case 202: return ACCEPTED;
			case 204: return NO_CONTENT;
			case 301: return MOVED_PERMANENTLY;
			case 302: return FOUND;
			case 303: return SEE_OTHER;
			case 304: return NOT_MODIFIED;
			case 400: return BAD_REQUEST;
			case 401: return UNAUTHORIZED;
			case 403: return FORBIDDEN;
			case 404: return NOT_FOUND;
			case 405: return METHOD_NOT_ALLOWED;
			case 406: return NOT_ACCEPTABLE;
			case 408: return REQUEST_TIMEOUT;
			case 409: return CONFLICT;
			case 410: return GONE;
			case 411: return LENGTH_REQUIRED;
			case 412: return PRECONDITION_FAILED;
			case 413: return PAYLOAD_TOO_LARGE;
			case 414: return URI_TOO_LONG;
			case 415: return UNSUPPORTED_MEDIA_TYPE;
			case 416: return RANGE_NOT_SATISFIABLE;
			case 417: return EXPECTATION_FAILED;
			case 418: return IM_A_TEAPOT;
			case 421: return MISDIRECTED_REQUEST;
			case 426: return UPGRADE_REQUIRED;
			case 500: return INTERNAL_SERVER_ERROR;
			case 501: return NOT_IMPLEMENTED;
			case 502: return BAD_GATEWAY;
			case 503: return SERVICE_UNAVAILABLE;
			case 504: return GATEWAY_TIMEOUT;
			case 505: return HTTP_VERSION_NOT_SUPPORTED;
			case 508: return LOOP_DETECTED;
			default: throw std::invalid_argument("Invalid status code.");
		}
	}

	const char* methodToString(Method method) {
		switch (method) {
			case GET: return "GET";
			case POST: return "POST";
			case DELETE: return "DELETE";
		}
		throw HttpException(INTERNAL_SERVER_ERROR, "unreachable");
	}

	Method stringToMethod(const std::string& method) {
		if (method == "GET") return GET;
		if (method == "POST") return POST;
		if (method == "DELETE") return DELETE;
		throw HttpException(NOT_IMPLEMENTED, "method not implemented");
	}

} // namespace http
