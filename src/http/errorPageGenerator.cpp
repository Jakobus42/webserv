#include "http/errorPageGenerator.hpp"

#include "http/StatusMessages.hpp"
#include "shared/defines.hpp"

#include <sstream>

namespace http {
	/**
	 * @brief Allocates the error page message data.
	 * @return std::map<int, std::string> The error page data.
	 */

	/**
	 * @brief Generates an error page.
	 * @param code The error code.
	 * @return std::string The error page as an HTML string.
	 */
	std::string generateErrorPage(StatusCode code) {
		const t_statusMessages& statusMessages = StatusMessages::getInstance().getStatusMessages();
		std::string message;

		try {
			message = statusMessages.at(code);
		} catch (...) {
			message = "Oopsie";
		}
		std::ostringstream oss;
		oss << code;
		std::string codeStr = oss.str();

		std::stringstream ss("");

		ss << "<!DOCTYPE html><html lang=\"en\"><head><title>" << codeStr << ' ' << message;
		ss << "</title><style>body{background-color:";
		ss << "#2b3042;justify-content: center;text-align: center;color: ";
		ss << "#d3dbeb;}h1{font-size: ";
		ss << "5rem;}p{font-size: 1.5rem;padding-bottom: ";
		ss << "10px;}a{text-decoration: none;color: ";
		ss << "#d3dbeb;padding: 10px;border: 3px solid #d3dbeb;font-weight: ";
		ss << "bold;}</style></head><body><h1>";
		ss << codeStr << "</h1><p>" << message;
		ss << "</p><a href=\"/home_directory>\">Go Back to Home</a></body></html>";


		std::string errorPage = ss.str();
		return errorPage;
	}

} // namespace http