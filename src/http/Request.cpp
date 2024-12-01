#include "../../include/http/Request.hpp"

#include <limits>

namespace http {

/**
 * @brief Constructs a new Request object.
 */
Request::Request()
    : m_head(""), m_body(""), m_content_length(std::numeric_limits<uint64_t>::max()), m_header_fields() {}

/**
 * @brief Destroys the Request object.
 */
Request::~Request() {}

/**
 * @brief Copy constructor.
 * @param other The other Request object to copy.
 */
Request::Request(const Request& other) {
  m_body = other.getBody();
  m_head = other.getHead();
  m_content_length = other.getContentLength();
  m_header_fields = other.getHeaderFields();
}

/**
 * @brief Copy assignment operator.
 * @param other The other Request object to assign from.
 * @return A reference to the assigned Request object.
 */
Request& Request::operator=(const Request& rhs) {
  if (this == &rhs) return *this;
  m_body = rhs.getBody();
  m_head = rhs.getHead();
  m_content_length = rhs.getContentLength();
  m_header_fields = rhs.getHeaderFields();
  return *this;
}

const std::string& Request::getBody(void) const { return m_body; }

const std::string& Request::getHead(void) const { return m_head; }

uint64_t Request::getContentLength(void) const { return m_content_length; }

const fields& Request::getHeaderFields(void) const { return m_header_fields; }

fields& Request::getHeaderFields(void) { return m_header_fields; }

void Request::setHeaderFields(fields& fields) { m_header_fields = fields; };

}  // namespace http
