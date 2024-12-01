#pragma once

#include <stdint.h>

#include <map>
#include <string>

typedef std::pair<std::string, std::string> t_field;
typedef std::map<t_field::first_type, t_field::second_type> t_fields;

namespace www {

/**
 * @class HttpRequest
 * @brief ...
 */
class HttpRequest {
 public:
  HttpRequest();
  ~HttpRequest();
  HttpRequest(const HttpRequest& other);
  HttpRequest& operator=(const HttpRequest& other);

  const std::string& getHead(void) const;
  const std::string& getBody(void) const;
  uint64_t getContentLength(void) const;
  const t_fields& getHeaderFields(void) const;

 private:
  std::string m_head;
  std::string m_body;
  std::string m_method;
  std::string m_path;

  uint64_t m_content_length;
  t_fields m_header_fields;
  // delimiter
};

} /* namespace www */
