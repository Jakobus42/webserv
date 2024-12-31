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
		const t_statusMessages& errorPages = StatusMessages::getInstance().getStatusMessages();
		if (errorPages.find(code) == errorPages.end()) {
			return "";
		}
		std::ostringstream oss;
		oss << code;
		std::string codeStr = oss.str();
		std::string errorPage =
			"<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>\n" + codeStr +
			" " + errorPages.at(code) +
			"\n</title>\n<style>\nbody\n{\nbackground-color:"
			"#2b3042;\njustify-content: center;\ntext-align: center;\ncolor: "
			"#d3dbeb;\n\n}\nh1\n{\nfont-size: "
			"5rem;\n}\np\n{\nfont-size: 1.5rem;\npadding-bottom: "
			"10px;\n}\na\n{\ntext-decoration: none;\ncolor: "
			"#d3dbeb;\npadding: 10px;\nborder: 3px solid #d3dbeb;\nfont-weight: "
			"bold;\n}\n</style>\n</head>\n<body>\n<h1>" +
			codeStr + "</h1>\n<p>\n" + errorPages.at(code) +
			"\n</p>\n<a href=\"/home_directory>\">\nGo Back to "
			"Home\n</a>\n</body>\n</html>";
		return errorPage;
	}

} // namespace http