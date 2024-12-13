#include "http/createResponse.hpp"

#include "http/ErrorMessages.hpp"

const std::string HTTP_NAME = "HTTP/";
const std::string ONE_DOT_ONE = "1.1";
const std::string CRLF = "\r\n";

namespace http {
	// this is the best response builder
	// there are many like it, but not one to match its excellency

	const std::string status(http::StatusCode status_code) {
		return ErrorMessages::getInstance().getErrorMessages().at(status_code);
	}

	const std::string createResponse(StatusCode status_code) {
		std::string response;

		std::string start_line;
		start_line = HTTP_NAME + ONE_DOT_ONE + " " + status(status_code) + CRLF;

		return response;
	}

} // namespace http
