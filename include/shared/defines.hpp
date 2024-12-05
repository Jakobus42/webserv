#pragma once

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#include <iostream>
#include <sstream>

typedef struct epoll_event t_event;

enum LogLevel {
	INFO = 0,
	WARNING,
	ERROR,
	DEBUG
}

#define LOG(message, level)                                           \
	std::ostringstream oss;                                           \
	oss << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": " \
		<< message;                                                   \
	std::cout << oss << std::endl;
;

namespace http {

const uint32_t LOCALHOST_ADDRESS = 0x7F000001;	 // 127.0.0.1
const uint32_t DEFAULT_MAX_BODY_SIZE = 1048576;	 // 1MB

/**
 * @enum HttpStatusCode
 * @brief Represents the various standard HTTP status codes.
 */
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
	HTTP_VERSION_NOT_SUPPORTED = 505
};

/**
 * @enum HttpMethod
 * @brief Represents the various HTTP methods
 */
enum Method {
	GET,
	POST,
	PUT,
	DELETE,
	PATCH,
	HEAD,
	OPTIONS,
	TRACE,
	CONNECT
};

}  // namespace http
