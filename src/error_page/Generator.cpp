#include "../../include/error_page/Generator.hpp"

#include "sstream"

namespace error_page {

/**
 * @brief Generates an error page.
 * @param code The error code.
 * @return std::string The error page.
 */
// variadic function
std::map<int, std::string> allocateErrorPageData() {
  std::map<int, std::string> errorPages;
  errorPages[400] = "Bad Request";
  errorPages[401] = "Unauthorized";
  errorPages[403] = "Forbidden";
  errorPages[404] = "Not Found";
  errorPages[405] = "Method Not Allowed";
  errorPages[406] = "Not Acceptable";
  errorPages[408] = "Request Timeout";
  errorPages[409] = "Conflict";
  errorPages[410] = "Gone";
  errorPages[411] = "Length Required";
  errorPages[413] = "Payload Too Large";
  errorPages[414] = "URI Too Long";
  errorPages[415] = "Unsupported Media Type";
  errorPages[416] = "Range Not Satisfiable";
  errorPages[417] = "Expectation Failed";
  errorPages[421] = "Misdirected Request";
  errorPages[426] = "Upgrade Required";
  errorPages[500] = "Internal Server Error";
  errorPages[501] = "Not Implemented";
  errorPages[502] = "Bad Gateway";
  errorPages[503] = "Service Unavailable";
  errorPages[504] = "Gateway Timeout";
  errorPages[505] = "HTTP Version Not Supported";
  return errorPages;
}

std::string generateErrorPage(int code) {
  std::map<int, std::string> errorPages;
  // TODO: Define the error page once at the beginning of the program
  errorPages = allocateErrorPageData();
  if (errorPages.find(code) == errorPages.end()) {
    return NULL;
  }
  std::ostringstream oss;
  oss << code;
  std::string codeStr = oss.str();
  std::string errorPage =
      "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>\n" + codeStr + " " + errorPages[code] +
      "\n</title>\n<style>\nbody\n{\nbackground-color:"
      "#2b3042;\njustify-content: center;\ntext-align: center;\ncolor: #d3dbeb;\n\n}\nh1\n{\nfont-size: "
      "5rem;\n}\np\n{\nfont-size: 1.5rem;\npadding-bottom: 10px;\n}\na\n{\ntext-decoration: none;\ncolor: "
      "#d3dbeb;\npadding: 10px;\nborder: 3px solid #d3dbeb;\nfont-weight: bold;\n}\n</style>\n</head>\n<body>\n<h1>" +
      codeStr + "</h1>\n<p>\n" + errorPages[code] +
      "\n</p>\n<a href=\"/home_directory>\">\nGo Back to Home\n</a>\n</body>\n</html>";
  return errorPage;
}

} /* namespace error_page */

/*<!DOCTYPE html>\n<html lang="en">\n<head>\n<title>\n404 Page Not Found\n</title>\n<style>\nbody\n{\nbackground-color:
 * #2b3042;\njustify-content: center;\ntext-align: center;\ncolor: #d3dbeb;\n\n}\nh1\n{\nfont-size:
 * 5rem;\n}\np\n{\nfont-size: 1.5rem;\npadding-bottom: 10px;\n}\na\n{\ntext-decoration: none;\ncolor: #d3dbeb;\npadding:
 * 10px;\nborder: 3px solid #d3dbeb;\nfont-weight: bold;\n}\n</style>\n</head>\n<body>\n<h1> 404 </h1>\n<p>\nOops! The
 * page you're\nlooking for is not here.\n</p>\n<a href="/home_directory>">\nGo Back to Home\n</a>\n</body>\n</html>*/