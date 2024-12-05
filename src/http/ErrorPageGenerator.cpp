#include "http/ErrorPageGenerator.hpp"

#include "http/defines.hpp"

// #include "http/defines.hpp"
#include "sstream"

namespace http {

/**
 * @brief Allocates the error page message data.
 * @return std::map<int, std::string> The error page data.
 */
std::map<StatusCode, std::string> allocateErrorPageData() {
  std::map<StatusCode, std::string> errorPages;
  errorPages[BAD_REQUEST] = "Bad Request";
  errorPages[UNAUTHORIZED] = "Unauthorized";
  errorPages[FORBIDDEN] = "Forbidden";
  errorPages[NOT_FOUND] = "Not Found";
  errorPages[METHOD_NOT_ALLOWED] = "Method Not Allowed";
  errorPages[NOT_ACCEPTABLE] = "Not Acceptable";
  errorPages[REQUEST_TIMEOUT] = "Request Timeout";
  errorPages[CONFLICT] = "Conflict";
  errorPages[GONE] = "Gone";
  errorPages[LENGTH_REQUIRED] = "Length Required";
  errorPages[PAYLOAD_TOO_LARGE] = "Payload Too Large";
  errorPages[URI_TOO_LONG] = "URI Too Long";
  errorPages[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
  errorPages[RANGE_NOT_SATISFIABLE] = "Range Not Satisfiable";
  errorPages[EXPECTATION_FAILED] = "Expectation Failed";
  errorPages[IM_A_TEAPOT] = "I'm a teapot";
  errorPages[MISDIRECTED_REQUEST] = "Misdirected Request";
  errorPages[UPGRADE_REQUIRED] = "Upgrade Required";
  errorPages[INTERNAL_SERVER_ERROR] = "Internal Server Error";
  errorPages[NOT_IMPLEMENTED] = "Not Implemented";
  errorPages[BAD_GATEWAY] = "Bad Gateway";
  errorPages[SERVICE_UNAVAILABLE] = "Service Unavailable";
  errorPages[GATEWAY_TIMEOUT] = "Gateway Timeout";
  errorPages[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
  return errorPages;
}

/**
 * @brief Generates an error page.
 * @param code The error code.
 * @return std::string The error page as an HTML string.
 */
std::string generateErrorPage(StatusCode code) {
  std::map<StatusCode, std::string> errorPages;
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

}  // namespace http