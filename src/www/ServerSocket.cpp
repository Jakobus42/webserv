#include "../../include/www/ServerSocket.hpp"

#include <iostream>
#include <vector>

namespace www {

typedef struct sockaddr t_sockaddr;

/**
 * @brief Constructs a new ServerSocket object for port 80.
 */
ServerSocket::ServerSocket()
    : m_fd(-1), m_epoll_fd(-1), m_port(80), m_open(false), m_bound(false), m_listening(false), m_epoll_alive(false) {}

/**
 * @brief Constructs a new ServerSocket object for a specified port.
 */
ServerSocket::ServerSocket(int port)
    : m_fd(-1), m_epoll_fd(-1), m_port(port), m_open(false), m_bound(false), m_listening(false), m_epoll_alive(false) {
  // port better not be negative
}

/**
 * @brief Destroys the ServerSocket object.
 */
ServerSocket::~ServerSocket() {
  //   this->close();
}

/**
 * @brief Copy constructor.
 * @param other The other ServerSocket object to copy.
 */
ServerSocket::ServerSocket(const ServerSocket& other) { operator=(other); }

/**
 * @brief Copy assignment operator.
 * @param other The other ServerSocket object to assign from.
 * @throws
 * @return A reference to the assigned ServerSocket object.
 */
ServerSocket& ServerSocket::operator=(const ServerSocket& rhs) {
  if (this == &rhs) return *this;
  m_fd = rhs.getFd();
  m_epoll_fd = rhs.getEpollFd();
  m_socketAddress = rhs.getSocketAddress();
  m_port = rhs.getPort();
  m_open = rhs.isOpen();
  m_bound = rhs.isBound();
  m_listening = rhs.isListening();
  m_epoll_alive = rhs.isEpollAlive();
  return *this;
}

int ServerSocket::getFd() const { return m_fd; }

int ServerSocket::getEpollFd() const { return m_epoll_fd; }

int ServerSocket::getPort() const { return m_port; }

t_sockaddr_in ServerSocket::getSocketAddress() const { return m_socketAddress; }

bool ServerSocket::isOpen(void) const { return m_open; }

bool ServerSocket::isBound(void) const { return m_bound; }

bool ServerSocket::isListening(void) const { return m_listening; }

bool ServerSocket::isEpollAlive(void) const { return m_epoll_alive; }

bool ServerSocket::init() { return (this->create() && this->bind() && this->listen()); }

bool ServerSocket::create() {
  int opt = 1;

  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) return false;
  if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    this->close();
    return false;
  }
  return true;
}

bool ServerSocket::bind() {
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port = htons(m_port);
  m_socketAddress.sin_addr.s_addr = htonl(LOCALHOST_ADDRESS);  // 127.0.0.1
  if (::bind(m_fd, (t_sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0) {
    this->close();
    return false;
  }
  m_bound = true;
  return true;
}

bool ServerSocket::listen() {
  if (::listen(m_fd, 10) < 0) {
    this->close();
    return false;
  }
  m_listening = true;
  return true;
}

bool ServerSocket::createEpollThingy() {
  m_epoll_fd = epoll_create(0);
  if (m_epoll_fd < 0) return false;
  m_epoll_alive = true;
  return true;
}

void ServerSocket::close(void) {
  ::close(m_fd);
  ::close(m_epoll_fd);
  m_fd = -1;
  this->setBroken();
}

void ServerSocket::setBroken(void) {
  m_open = false;
  m_bound = false;
  m_listening = false;
}

} /* namespace www */
