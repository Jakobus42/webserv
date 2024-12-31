#include "http/createResponse.hpp"

// this will probably be replaced by the Response class

#include "http/StatusMessages.hpp"

const std::string HTTP_NAME = "HTTP/";
const std::string ONE_DOT_ONE = "1.1";
const std::string CRLF = "\r\n";

namespace http {
	// this is the best response builder
	// there are many like it, but not one to match its excellency

	const std::string status(http::StatusCode status_code) {
		return StatusMessages::getInstance().getStatusMessages().at(status_code);
	}

	const std::string createResponse(StatusCode status_code) {
		std::string response;

		std::string start_line;
		start_line = HTTP_NAME + ONE_DOT_ONE + " " + status(status_code) + CRLF;

		response += start_line;

		return response;
	}

} // namespace http
