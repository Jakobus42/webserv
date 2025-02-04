#pragma once

#include <string>
#include <vector>

namespace http {
	/**
	 * @enum HttpStatusCode
	 * @brief Represents the various standard HTTP status codes.
	 */
	enum StatusCode {
		UNKNWN = 0,
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

	/**
	 * @enum HttpMethod
	 * @brief Represents the various HTTP methods
	 */
	enum Method {
		GET,
		POST,
		DELETE,
		_ERROR
	};

	enum RequestType {
		FETCH,
		CGI
	};

	enum FileType {
		FILE,
		DIRECTORY,
		_NOT_FOUND
	};

	struct Uri {
			std::string scheme;
			std::string authority;
			std::string path;
			std::vector<std::string> pathSegments;
			std::string safeAbsolutePath;
			std::string query;
			std::string cgiPathInfo;

			Uri()
				: scheme()
				, authority()
				, path()
				, pathSegments()
				, safeAbsolutePath()
				, query()
				, cgiPathInfo() {}

			bool isAbsoluteForm() {
				return path.length() > 0 ? path[0] != '/' : false;
			}
	};
} // namespace http
