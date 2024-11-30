#include "../../include/www/Socket.hpp"

#include <iostream>
#include <vector>

namespace www {

const uint32_t LOCALHOST_ADDRESS = 0x7F000001;  // 127.0.0.1

/**
 * @brief Constructs a new Socket object for port 80.
 */
Socket::Socket() : m_fd(-1), m_port(80), m_open(false), m_bound(false), m_listening(false) {}

/**
 * @brief Constructs a new Socket object for a specified port.
 */
Socket::Socket(int port) : m_fd(-1), m_port(port), m_open(false), m_bound(false), m_listening(false) {
  // port better not be negative
}

/**
 * @brief Destroys the Socket object.
 */
Socket::~Socket() {
  //   this->close();
}

/**
 * @brief Copy constructor.
 * @param other The other Socket object to copy.
 */
Socket::Socket(const Socket& other) { operator=(other); }

/**
 * @brief Copy assignment operator.
 * @param other The other Socket object to assign from.
 * @throws
 * @return A reference to the assigned Socket object.
 */
Socket& Socket::operator=(const Socket& rhs) {
  if (this == &rhs) return *this;
  m_fd = rhs.getFd();
  m_port = rhs.getPort();
  return *this;
}

int Socket::getFd() const { return m_fd; }

int Socket::getPort() const { return m_port; }

bool Socket::isOpen(void) const { return m_open; }

bool Socket::isBound(void) const { return m_bound; }

bool Socket::isListening(void) const { return m_listening; }

bool Socket::init() { return (this->create() && this->bind() && this->listen()); }

bool Socket::create() {
  int opt = 1;

  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) return false;
  if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    this->close();
    return false;
  }
  return true;
}

bool Socket::bind() {
  struct sockaddr_in in_address;

  in_address.sin_family = AF_INET;
  in_address.sin_port = htons(m_port);
  in_address.sin_addr.s_addr = htonl(LOCALHOST_ADDRESS);  // 127.0.0.1
  if (::bind(m_fd, (struct sockaddr*)&in_address, sizeof(in_address)) < 0) {
    this->close();
    return false;
  }
  m_bound = true;
  return true;
}

bool Socket::listen() {
  if (::listen(m_fd, 10) < 0) {
    this->close();
    return false;
  }
  m_listening = true;
  return true;
}

void Socket::close(void) {
  ::close(m_fd);
  m_fd = -1;
  this->setBroken();
}

void Socket::setBroken(void) {
  m_open = false;
  m_bound = false;
  m_listening = false;
}

} /* namespace www */
