#include "http/Request.hpp"

#include <strings.h>

namespace http {

/**
 * @brief Constructs a new Request object.
 */
Request::Request()
    : m_read_buffer(), m_received_bytes(0), m_method(http::GET), m_uri("/"), m_data(""), m_head(""), m_body("") {}

/**
 * @brief Destroys the Request object.
 */
Request::~Request() {}

/**
 * @brief Copy constructor.
 * @param other The other Request object to copy.
 */
Request::Request(const Request& other)
    : m_read_buffer(),
      m_received_bytes(other.getReceivedBytes()),
      m_data(other.getData()),
      m_head(other.getHead()),
      m_body(other.getBody()) {}

/**
 * @brief Copy assignment operator.
 * @param other The other Request object to assign from.
 * @return A reference to the assigned Request object.
 */
Request& Request::operator=(const Request& rhs) {
  if (this == &rhs) return *this;
  m_data = rhs.getData();
  m_head = rhs.getHead();
  m_body = rhs.getBody();
  m_received_bytes = rhs.getReceivedBytes();
  bzero(m_read_buffer, sizeof(m_read_buffer));
  return *this;
}

uint32_t Request::getReceivedBytes(void) const { return m_received_bytes; };
const std::string& Request::getData(void) const { return m_data; };
const std::string& Request::getHead(void) const { return m_head; };
const std::string& Request::getBody(void) const { return m_body; };
Method Request::getMethod(void) const { return m_method; }
const std::string& Request::getUri(void) const { return m_uri; }

} /* namespace http */
