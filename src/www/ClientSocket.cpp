#include "www/ClientSocket.hpp"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

namespace www {

/**
 * @brief Constructs a new ClientSocket object.
 */
ClientSocket::ClientSocket() : m_fd(-1), m_socketAddress(), m_socketSize(), m_alive(false) {}

/**
 * @brief Constructs a new ClientSocket object.
 */
ClientSocket::ClientSocket(int listen_socket) throw(std::exception)
    : m_fd(-1), m_socketAddress(), m_socketSize(), m_alive(false) {
  this->accept(listen_socket);
}

/**
 * @brief Destroys the ClientSocket object.
 * @warning Does not close the FD (for a reason), close manually or it leaks!
 * @note Could add a flag close_on_destroy that is checked in the destructor.
 */
ClientSocket::~ClientSocket() {}

/**
 * @brief Copy constructor.
 * @param other The other ClientSocket object to copy.
 */
ClientSocket::ClientSocket(const ClientSocket& other)
    : m_fd(other.getFd()),
      m_socketAddress(other.getSocketAddress()),
      m_socketSize(other.getSocketSize()),
      m_alive(other.isAlive()) {}

/**
 * @brief Copy assignment operator.
 * @param other The other ClientSocket object to assign from.
 * @return A reference to the assigned ClientSocket object.
 */
ClientSocket& ClientSocket::operator=(const ClientSocket& rhs) {
  if (this == &rhs) return *this;
  m_fd = rhs.getFd();
  m_socketAddress = rhs.getSocketAddress();
  m_socketSize = rhs.getSocketSize();
  m_alive = rhs.isAlive();
  return *this;
}

bool ClientSocket::operator==(const ClientSocket& other) const { return (other.getFd() == this->getFd()); }

int ClientSocket::getFd(void) const { return m_fd; }

int ClientSocket::getFd(void) { return m_fd; }

t_sockaddr ClientSocket::getSocketAddress(void) const { return m_socketAddress; }

uint32_t ClientSocket::getSocketSize(void) const { return m_socketSize; }

bool ClientSocket::isAlive(void) const { return m_alive; }

void ClientSocket::close(void) {
  ::close(m_fd);
  m_fd = -1;
  m_alive = false;
}

void ClientSocket::accept(int listen_socket) throw(std::exception) {
  bzero(&m_socketAddress, sizeof(t_sockaddr));
  m_socketSize = 0;
  std::cout << "Accepting connection for socket " << listen_socket << std::endl;
  m_fd = ::accept(listen_socket, &m_socketAddress, &m_socketSize);
  if (m_fd < 0) {
    std::cout << "Shit, accept didn't work" << errno << std::endl;
    throw std::exception();
  }
  m_alive = true;
}

} /* namespace www */
