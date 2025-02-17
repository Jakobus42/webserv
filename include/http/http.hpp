#pragma once

#include <dirent.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#include "config/Location.hpp"
#include "http/types.hpp"
#include "shared/Logger.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace http {

	const uint32_t LOCALHOST_ADDRESS = 0x7F000001; // 127.0.0.1

	const std::string HTTP_VERSION = "HTTP/1.1";
	const std::string CRLF = "\r\n";

	// Content-Type
	const std::string TEXT_HTML = "text/html";

	class exception : public std::runtime_error {
		public:
			explicit exception(StatusCode statusCode);
			exception(StatusCode statusCode, const std::string& message);
			virtual ~exception() throw();

			StatusCode getStatusCode() const;
			const std::string& getMessage() const;

		private:
			std::string buildErrorMessage(StatusCode statusCode, const std::string& message);

		private:
			StatusCode m_statusCode;
			std::string m_message;
	};

	const std::string& getStatusMessage(StatusCode statusCode);
	std::string getErrorPage(StatusCode statusCode);
	std::string getMethodString(Method method);
	Method stringToMethod(const std::string& method);
	std::string getDirectoryListing(const Uri& uri, const config::Location& location, const std::string& root);
	bool getRootPath(std::string& rootPath);

} // namespace http
