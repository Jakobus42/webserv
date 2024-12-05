#include "error_page/Generator.hpp"

#include "http/defines.hpp"

// #include "http/defines.hpp"
#include "sstream"

namespace error_page {

/**
 * @brief Allocates the error page message data.
 * @return std::map<int, std::string> The error page data.
 */
std::map<int, std::string> allocateErrorPageData() {
  std::map<int, std::string> errorPages;
  errorPages[http::BAD_REQUEST] = "Bad Request";
  errorPages[http::UNAUTHORIZED] = "Unauthorized";
  errorPages[http::FORBIDDEN] = "Forbidden";
  errorPages[http::NOT_FOUND] = "Not Found";
  errorPages[http::METHOD_NOT_ALLOWED] = "Method Not Allowed";
  errorPages[http::NOT_ACCEPTABLE] = "Not Acceptable";
  errorPages[http::REQUEST_TIMEOUT] = "Request Timeout";
  errorPages[http::CONFLICT] = "Conflict";
  errorPages[http::GONE] = "Gone";
  errorPages[http::LENGTH_REQUIRED] = "Length Required";
  errorPages[http::PAYLOAD_TOO_LARGE] = "Payload Too Large";
  errorPages[http::URI_TOO_LONG] = "URI Too Long";
  errorPages[http::UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
  errorPages[http::RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
  errorPages[http::EXPECTATION_FAILED] = "Expectation Failed";
  errorPages[http::IM_A_TEAPOT] = "I'm a teapot";
  errorPages[http::MISDIRECTED_REQUEST] = "Misdirected Request";
  errorPages[http::UPGRADE_REQUIRED] = "Upgrade Required";
  errorPages[http::INTERNAL_SERVER_ERROR] = "Internal Server Error";
  errorPages[http::NOT_IMPLEMENTED] = "Not Implemented";
  errorPages[http::BAD_GATEWAY] = "Bad Gateway";
  errorPages[http::SERVICE_UNAVAILABLE] = "Service Unavailable";
  errorPages[http::GATEWAY_TIMEOUT] = "Gateway Timeout";
  errorPages[http::HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
  return errorPages;
}

/**
 * @brief Generates an error page.
 * @param code The error code.
 * @return std::string The error page as an HTML string.
 */
std::string generateErrorPage(int code) {
  std::map<int, std::string> errorPages;
  // TODO: Define the error page once at the beginning of the program
  errorPages = allocateErrorPageData();
  if (errorPages.find(code) == errorPages.end()) {
    return "";
  }
  std::ostringstream oss;
  oss << code;
  std::string codeStr = oss.str();
  std::string errorPage = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>\n" + codeStr + " " + errorPages[code] +
                          "\n</title>\n<style>\nbody\n{\nbackground-color:"
                          "#2b3042;\njustify-content: center;\ntext-align: center;\ncolor: "
                          "#d3dbeb;\n\n}\nh1\n{\nfont-size: "
                          "5rem;\n}\np\n{\nfont-size: 1.5rem;\npadding-bottom: "
                          "10px;\n}\na\n{\ntext-decoration: none;\ncolor: "
                          "#d3dbeb;\npadding: 10px;\nborder: 3px solid #d3dbeb;\nfont-weight: "
                          "bold;\n}\n</style>\n</head>\n<body>\n<h1>" +
                          codeStr + "</h1>\n<p>\n" + errorPages[code] +
                          "\n</p>\n<a href=\"/home_directory>\">\nGo Back to "
                          "Home\n</a>\n</body>\n</html>";
  return errorPage;
}

} /* namespace error_page */