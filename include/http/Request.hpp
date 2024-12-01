#pragma once

#include <stdint.h>

#include <map>
#include <string>

namespace http {

typedef std::pair<std::string, std::string> field;
typedef std::map<field::first_type, field::second_type> fields;

/**
 * @class Request
 * @brief ...
 */
class Request {
 public:
  Request();
  ~Request();
  Request(const Request& other);
  Request& operator=(const Request& other);

  const std::string& getHead(void) const;
  const std::string& getBody(void) const;
  uint64_t getContentLength(void) const;
  const fields& getHeaderFields(void) const;
  fields& getHeaderFields(void);

  void setHeaderFields(fields&);

 private:
  std::string m_head;
  std::string m_body;
  std::string m_method;
  std::string m_path;

  uint64_t m_content_length;
  fields m_header_fields;
  // delimiter
};

}  // namespace http
