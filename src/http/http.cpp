#include "http/http.hpp"

#include "shared/stringUtils.hpp"

namespace http {

	exception::exception(StatusCode statusCode)
		: std::runtime_error(shared::string::toString(static_cast<int>(statusCode)) + " - " + std::string(statusCodeToMessage(statusCode)))
		, m_statusCode(statusCode)
		, m_message(statusCodeToMessage(statusCode)) {
	}

	exception::exception(StatusCode statusCode, const std::string& message)
		: std::runtime_error(shared::string::toString(static_cast<int>(statusCode)) + " - " + message)
		, m_statusCode(statusCode)
		, m_message(message) {
	}

	exception::~exception() throw() {}

	StatusCode exception::getStatusCode() const { return m_statusCode; }

	const std::string& exception::getMessage() const { return m_message; }

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
		}
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
			default: return INTERNAL_SERVER_ERROR;
		}
	}

	const char* methodToString(Method method) {
		switch (method) {
			case GET: return "GET";
			case POST: return "POST";
			case DELETE: return "DELETE";
		}
	}

	Method stringToMethod(const std::string& method) {
		if (method == "GET") return GET;
		if (method == "POST") return POST;
		if (method == "DELETE") return DELETE;
		throw http::exception(NOT_IMPLEMENTED, "method not implemented");
	}

	// todo: move this somewhere else

	// /**
	//  * @brief Generate the Directory Listing HTML for the passed uri
	//  *
	//  * @param uri
	//  * @param root
	//  * @return std::string the rendered HTML for the directory.
	//  * @note In case of a location's root having a subdirectory that shares the name of a sublocation, the location takes precedence during routing.
	//  * This means upon autoindex, clicking the directory foo/ routes to a nested location named /foo, and foo/ is inaccessible. This is by design.
	//  */
	// std::string getDirectoryListing(const Uri& uri, const config::Location& location) { // temporarily pass root path as parameter
	// 	static const char* DL_TEMPLATE = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
	// 	"content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title><style>body{font-family:Arial,sans-serif;"
	// 									 "margin:0;padding:0;background-color:#f8f9fa;}.container{max-width:800px;margin:50px auto;padding:20px;background:#fff;box-shadow:0 "
	// 									 "4px 6px rgba(0,0,0,0.1);border-radius:8px;}h1{font-size:1.8rem;color:#333;text-align:center;}ul{list-style-type:none;padding:0;}"
	// 									 "li{padding:8px 10px;border-bottom:1px solid #e0e0e0;}li a{text-decoration:none;color:#007bff;transition:color 0.3s;}"
	// 									 "li a:hover{color:#0056b3;}</style></head><body><div class=\"container\"><h1>Directory Listing</h1><ul>";
	// 									 static const char* DL_TEMPLATE_END = "</ul></div></body></html>";
	// 									 std::string bodyTemp = DL_TEMPLATE;

	// 	if (!shared::file::directoryExists(location.precalculatedAbsolutePath)) {
	// 		throw http::exception(NOT_FOUND, "getDirectoryListing: Directory doesn't exist: " + location.precalculatedAbsolutePath);
	// 	}

	// 	DIR* dir;
	// 	if ((dir = opendir(uri.safeAbsolutePath.c_str())) == NULL) {
	// 		throw http::exception(INTERNAL_SERVER_ERROR, "getDirectoryListing: Couldn't open directory");
	// 	}
	// 	struct dirent* ent;
	// 	while ((ent = readdir(dir)) != NULL) {
	// 		std::string fileName(ent->d_name);

	// 		if (fileName == ".") {
	// 			continue;
	// 		}
	// 		if (fileName == ".." && (uri.path == "/" || uri.path == "")) {
	// 			continue;
	// 		}
	// 		std::string baseUrl = uri.authority;
	// 		if (uri.path.empty()) {
	// 			baseUrl += "/";
	// 		} else {
	// 			// Ensure uri.path starts with '/'.
	// 			if (uri.path[0] != '/')
	// 				baseUrl += "/" + uri.path;
	// 			else
	// 			baseUrl += uri.path;
	// 			// Ensure it ends with a single '/'
	// 			if (baseUrl[baseUrl.size() - 1] != '/')
	// 			baseUrl += "/";
	// 		}

	// 		std::string link;
	// 		if (ent->d_type == DT_DIR) {
	// 			// Add trailing slash to show it’s a directory.
	// 			link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "/</a>";
	// 		} else {
	// 			link = "<a href=\"http://" + baseUrl + fileName + "\">" + fileName + "</a>";
	// 		}
	// 		// does '///' get normalized to '/' ? I think this still breaks things, so foo//bar isn't foo/bar
	// 		bodyTemp += "<li>" + link + "</li>";
	// 	}
	// 	closedir(dir);
	// 	return bodyTemp + DL_TEMPLATE_END;
	// }

	// std::string statusCodeToErrorPage(StatusCode statusCode) {
	// 	static const char* errorPageTemplate =
	// 		"<!DOCTYPE html><html lang=\"en\"><head><title>%d %s</title><style>"
	// 		"body{background-color:#2b3042;justify-content: center;text-align: center;color:#d3dbeb;}"
	// 		"h1{font-size:5rem;}p{font-size: 1.5rem;padding-bottom: 10px;}a{"
	// 		"text-decoration: none;color: #d3dbeb;padding: 10px;border: 3px solid #d3dbeb;font-weight: bold;}</style>"
	// 		"</head><body><h1>%d</h1><p>%s</p><a href=\"/\">Go Back to Home</a></body></html>";

	// 	char errorPage[1024];
	// 	const char* statusMessage = getStatusMessage(statusCode).c_str();
	// 	std::cout << "getErrorPage writing: " << statusCode << " " << statusMessage << std::endl;
	// 	snprintf(errorPage, sizeof(errorPage), errorPageTemplate, statusCode, statusMessage, statusCode, statusMessage); // TODO: replace this with a legal function

	// 	return std::string(errorPage);
	// }

} // namespace http
