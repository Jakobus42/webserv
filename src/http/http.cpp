#include "http/http.hpp"

#include "config/Location.hpp"
#include "shared/fileUtils.hpp"

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
			statusMessages[LOOP_DETECTED] = "Loop Detected";
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
			case _ERROR:
				return "ERROR: You should not be seeing this";
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
			"</head><body><h1>%d</h1><p>%s</p><a href=\"/\">Go Back to Home</a></body></html>";

		char errorPage[1024];
		const char* statusMessage = getStatusMessage(statusCode).c_str();
		std::cout << "getErrorPage writing: " << statusCode << " " << statusMessage << std::endl;
		snprintf(errorPage, sizeof(errorPage), errorPageTemplate, statusCode, statusMessage, statusCode, statusMessage); // TODO: replace this with a legal function

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

	StatusCode exception::getStatusCode() const { return m_statusCode; }

	const std::string& exception::getMessage() const { return m_message; }

	std::string exception::buildErrorMessage(StatusCode statusCode, const std::string& message) {
		std::ostringstream oss;
		oss << static_cast<int>(statusCode) << " - " << message;
		return oss.str(); // Only one construction of the string
	}

	/**
	 * @brief Generate the Directory Listing HTML for the passed uri
	 *
	 * @param uri
	 * @param root
	 * @return std::string the rendered HTML for the directory.
	 * @note In case of a location's root having a subdirectory that shares the name of a sublocation, the location takes precedence during routing.
	 * This means upon autoindex, clicking the directory foo/ routes to a nested location named /foo, and foo/ is inaccessible. This is by design.
	 */
	std::string getDirectoryListing(const Uri& uri, const config::Location& location) { // temporarily pass root path as parameter
		static const char* DL_TEMPLATE = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
										 "content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title><style>body{font-family:Arial,sans-serif;"
										 "margin:0;padding:0;background-color:#f8f9fa;}.container{max-width:800px;margin:50px auto;padding:20px;background:#fff;box-shadow:0 "
										 "4px 6px rgba(0,0,0,0.1);border-radius:8px;}h1{font-size:1.8rem;color:#333;text-align:center;}ul{list-style-type:none;padding:0;}"
										 "li{padding:8px 10px;border-bottom:1px solid #e0e0e0;}li a{text-decoration:none;color:#007bff;transition:color 0.3s;}"
										 "li a:hover{color:#0056b3;}</style></head><body><div class=\"container\"><h1>Directory Listing</h1><ul>";
		static const char* DL_TEMPLATE_END = "</ul></div></body></html>";
		std::string bodyTemp = DL_TEMPLATE;

		if (!shared::file::directoryExists(location.precalculatedAbsolutePath)) {
			throw http::exception(NOT_FOUND, "getDirectoryListing: Directory doesn't exist: " + location.precalculatedAbsolutePath);
		}
		// if (!shared::file::dirIsAccessible(rootPath)) { // TODO: actually maybe not good? what if root isn't accessible but subdir is? is that even possible?
		// 	throw http::exception(FORBIDDEN, "getDirectoryListing: Directory isn't accessible: " + rootPath);
		// }
		DIR* dir;
		if ((dir = opendir(uri.safeAbsolutePath.c_str())) == NULL) {
			throw http::exception(INTERNAL_SERVER_ERROR, "getDirectoryListing: Couldn't open directory");
		}
		struct dirent* ent;
		while ((ent = readdir(dir)) != NULL) {
			std::string fileName(ent->d_name);

			if (fileName == ".") {
				continue;
			}
			if (fileName == ".." && (uri.path == "/" || uri.path == "")) {
				continue;
			}
			std::string baseUrl = uri.authority;
			if (uri.path.empty()) {
				baseUrl += "/";
			} else {
				// Ensure uri.path starts with '/'.
				if (uri.path[0] != '/')
					baseUrl += "/" + uri.path;
				else
					baseUrl += uri.path;
				// Ensure it ends with a single '/'
				if (baseUrl[baseUrl.size() - 1] != '/')
					baseUrl += "/";
			}

			std::string link;
			if (ent->d_type == DT_DIR) {
				// Add trailing slash to show it’s a directory.
				link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "/</a>";
			} else {
				link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "</a>";
			}
			// does '///' get normalized to '/' ? I think this still breaks things, so foo//bar isn't foo/bar
			bodyTemp += "<li>" + link + "</li>";
		}
		closedir(dir);
		return bodyTemp + DL_TEMPLATE_END;
	}

} // namespace http
