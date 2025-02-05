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
			"</head><body><h1>%d</h1><p>%s</p><a href=\"/home_directory\">Go Back to Home</a></body></html>";

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

	bool getDirectoryListing(const std::string& path, std::string& body) {
		static const char* DlTemplate = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\""
										"content=\"width=device-width, initial-scale=1.0\"><title>Directory Listing</title><style>body{font-family:Arial,sans-serif;"
										"margin:0;padding:0;background-color:#f8f9fa;}.container{max-width:800px;margin:50px auto;padding:20px;background:#fff;box-shadow:0 "
										"4px 6px rgba(0,0,0,0.1);border-radius:8px;}h1{font-size:1.8rem;color:#333;text-align:center;}ul{list-style-type:none;padding:0;}"
										"li{padding:8px 10px;border-bottom:1px solid #e0e0e0;}li a{text-decoration:none;color:#007bff;transition:color 0.3s;}"
										"li a:hover{color:#0056b3;}</style></head><body><div class=\"container\"><h1>Directory Listing</h1><ul>";
		static const char* DlTemplateEnd = "</ul></div></body></html>";
		std::string bodyTemp;

		bodyTemp = DlTemplate;
		DIR* dir;
		std::string rootPath;
		getRootPath(rootPath);
		/* 		if (rootPath[rootPath.size() - 1] != '/') {
					rootPath += "/";
				} */
		rootPath += path;
		if ((dir = opendir(rootPath.c_str())) != NULL) {
			struct dirent* ent;
			while ((ent = readdir(dir)) != NULL) {
				if (std::string(ent->d_name) == ".") {
					continue;
				}
				if (std::string(ent->d_name) == "..") {
					if (path == "/" || path == "") {
						continue;
					}
				}
				if (ent->d_type == DT_DIR) {
					std::string link = "<a href=\"" + std::string(ent->d_name) + "/" + "\">" + std::string(ent->d_name) + "/</a>";
					bodyTemp += "<li>" + link + "</li>";
					// std::cout << "Dir: " << ent->d_name << std::endl;
				} else {
					std::string link = "<a href=\"" + std::string(ent->d_name) + "\">" + std::string(ent->d_name) + "</a>";
					bodyTemp += "<li>" + link + "</li>";
				}
			}
			closedir(dir);
		} else {
			return false;
		}
		body = bodyTemp + DlTemplateEnd;
		return true;
	}

	bool getRootPath(std::string& path) {
		static std::string rootPath;
		if (!rootPath.empty()) {
			path = rootPath;
			return true;
		}
		DIR* dir;
		// get current directory
		char buffer[1024];
		if (getcwd(buffer, 1024) == NULL) {
			LOG("Error getting current directory", shared::FATAL);
			return false;
		}
		bool foundWWW = false;
		/* bool foundCGI = false; */
		if ((dir = opendir(buffer)) != NULL) {
			struct dirent* ent;
			while ((ent = readdir(dir)) != NULL) {
				if (std::string(ent->d_name) == "www") {
					rootPath = std::string(buffer) + "/www";
					foundWWW = true;
				}
				/* 				if (std::string(ent->d_name) == ".cgi")
								{
									foundCGI = true;
								} */
			}
			closedir(dir);
		} else {
			LOG("Error opening current directory", shared::FATAL);
			return false;
		}
		if (foundWWW) // && foundCGI)
		{
			return true;
		}
		LOG("Error finding www and .cgi directory", shared::FATAL);
		return false;
	}

} // namespace http