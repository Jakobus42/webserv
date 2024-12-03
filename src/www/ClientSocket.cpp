#include "../../include/www/ClientSocket.hpp"

#include <string.h>

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
 */
ClientSocket::~ClientSocket() {}

// /**
//  * @brief Copy constructor.
//  * @param other The other ClientSocket object to copy.
//  */
// ClientSocket::ClientSocket(const ClientSocket &) {
// }

// /**
//  * @brief Copy assignment operator.
//  * @param other The other ClientSocket object to assign from.
//  * @return A reference to the assigned ClientSocket object.
//  */
// ClientSocket &ClientSocket::operator=(const ClientSocket &) {
// 	return *this;
// }

int ClientSocket::getFd(void) const { return m_fd; }

t_sockaddr ClientSocket::getSocketAddress(void) const { return m_socketAddress; }

bool ClientSocket::isAlive(void) const { return m_alive; }

void ClientSocket::accept(int listen_socket) throw(std::exception) {
  bzero(&m_socketAddress, sizeof(t_sockaddr));
  m_fd = ::accept(listen_socket, &m_socketAddress, &m_socketSize);
  if (m_fd < 0) {
    std::cout << "Shit, accept didn't work" << std::endl;
    throw std::exception();
  }
  m_alive = true;
}

} /* namespace www */
