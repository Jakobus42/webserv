#include "../../include/www/HttpRequest.hpp"

#include <limits>

namespace www {

/**
 * @brief Constructs a new HttpRequest object.
 */
HttpRequest::HttpRequest()
    : m_head(""), m_body(""), m_content_length(std::numeric_limits<uint64_t>::max()), m_header_fields() {}

/**
 * @brief Destroys the HttpRequest object.
 */
HttpRequest::~HttpRequest() {}

/**
 * @brief Copy constructor.
 * @param other The other HttpRequest object to copy.
 */
HttpRequest::HttpRequest(const HttpRequest& other) {
  m_body = other.getBody();
  m_head = other.getHead();
  m_content_length = other.getContentLength();
  m_header_fields = other.getHeaderFields();
}

/**
 * @brief Copy assignment operator.
 * @param other The other HttpRequest object to assign from.
 * @return A reference to the assigned HttpRequest object.
 */
HttpRequest& HttpRequest::operator=(const HttpRequest& rhs) {
  if (this == &rhs) return *this;
  m_body = rhs.getBody();
  m_head = rhs.getHead();
  m_content_length = rhs.getContentLength();
  m_header_fields = rhs.getHeaderFields();
  return *this;
}

const std::string& HttpRequest::getBody(void) const { return m_body; }

const std::string& HttpRequest::getHead(void) const { return m_head; }
uint64_t HttpRequest::getContentLength(void) const { return m_content_length; }

const t_fields& HttpRequest::getHeaderFields(void) const { return m_header_fields; }

t_fields& HttpRequest::getHeaderFields(void) { return m_header_fields; }

void HttpRequest::setHeaderFields(t_fields& fields) { m_header_fields = fields; };

} /* namespace www */
