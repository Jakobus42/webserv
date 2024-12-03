#include "../../include/www/VirtualServerSocket.hpp"

#include <errno.h>

#include <iostream>
#include <vector>

namespace www {

typedef struct sockaddr t_sockaddr;

/**
 * @brief Constructs a new VirtualServerSocket object for port 80.
 */
VirtualServerSocket::VirtualServerSocket()
    : m_fd(-1), m_ip(0x7f000001), m_port(80), m_open(false), m_bound(false), m_listening(false) {}

/**
 * @brief Constructs a new VirtualServerSocket object for a specified port.
 */
VirtualServerSocket::VirtualServerSocket(int port, uint32_t ip)
    : m_fd(-1), m_ip(ip), m_port(port), m_open(false), m_bound(false), m_listening(false) {
  // port better not be negative
}

/**
 * @brief Destroys the VirtualServerSocket object.
 * @warning Does not close the FD (for a reason), close manually or it leaks!
 * @note Could add a flag close_on_destroy that is checked in the destructor.
 */
VirtualServerSocket::~VirtualServerSocket() {}

/**
 * @brief Copy constructor.
 * @param other The other VirtualServerSocket object to copy.
 */
VirtualServerSocket::VirtualServerSocket(const VirtualServerSocket& other) { operator=(other); }

/**
 * @brief Copy assignment operator.
 * @param other The other VirtualServerSocket object to assign from.
 * @throws
 * @return A reference to the assigned VirtualServerSocket object.
 */
VirtualServerSocket& VirtualServerSocket::operator=(const VirtualServerSocket& rhs) {
  if (this == &rhs) return *this;
  m_fd = rhs.getFd();
  m_socketAddress = rhs.getSocketAddress();
  m_port = rhs.getPort();
  m_ip = rhs.getIp();
  m_open = rhs.isOpen();
  m_bound = rhs.isBound();
  m_listening = rhs.isListening();
  return *this;
}

int VirtualServerSocket::getFd() const { return m_fd; }

int VirtualServerSocket::getPort() const { return m_port; }

uint32_t VirtualServerSocket::getIp() const { return m_ip; }

t_sockaddr_in VirtualServerSocket::getSocketAddress() const { return m_socketAddress; }

bool VirtualServerSocket::isOpen(void) const { return m_open; }

bool VirtualServerSocket::isBound(void) const { return m_bound; }

bool VirtualServerSocket::isListening(void) const { return m_listening; }

bool VirtualServerSocket::init() { return (this->create() && this->bind() && this->listen()); }

bool VirtualServerSocket::create(void) {
  int opt = 1;

  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  std::cout << "VirtualServerSocket Made" << std::endl;
  if (m_fd == -1) return false;
  if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::cout << "VirtualServerSocket options fucked" << std::endl;
    this->close();
    return false;
  }
  std::cout << "VirtualServerSocket should be good to go" << std::endl;
  return true;
}

bool VirtualServerSocket::bind() {
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port = htons(m_port);
  m_socketAddress.sin_addr.s_addr = htonl(m_ip);
  std::cout << "Binding to port " << m_port << ", address " << m_ip << std::endl;
  if (::bind(m_fd, (t_sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0) {
    std::cout << "Bind failed with " << errno << std::endl;
    this->close();
    return false;
  }
  m_bound = true;
  return true;
}

bool VirtualServerSocket::listen(void) {
  if (::listen(m_fd, MAX_EVENTS) < 0) {
    std::cout << "E1" << errno << std::endl;
    this->close();
    return false;
  }
  m_listening = true;
  std::cout << "ServerSocket is now listening" << std::endl;
  return true;
}

void VirtualServerSocket::close(void) {
  ::close(m_fd);
  m_fd = -1;
  this->setBroken();
}

void VirtualServerSocket::setBroken(void) {
  m_open = false;
  m_bound = false;
  m_listening = false;
}

} /* namespace www */
