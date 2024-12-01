#include "../../include/www/Client.hpp"

namespace www {

/**
 * @brief Constructs a new Client object.
 */
Client::Client() : m_fd(-1), m_socketAddress(), m_socketLength(0), m_alive(false) {}

/**
 * @brief Destroys the Client object.
 */
Client::~Client() {
  // this->close();
}

/**
 * @brief Copy constructor.
 * @param other The other Client object to copy.
 */
Client::Client(const Client& other) { operator=(other); }

/**
 * @brief Copy assignment operator.
 * @param other The other Client object to assign from.
 * @return A reference to the assigned Client object.
 */
Client& Client::operator=(const Client& rhs) {
  if (this == &rhs) return *this;
  m_fd = rhs.getFd();
  m_socketAddress = rhs.getSocketAddress();
  m_socketLength = rhs.getSocketLength();
  m_alive = rhs.isAlive();
  return *this;
}

int Client::getFd(void) const { return m_fd; }

t_sockaddr Client::getSocketAddress(void) const { return m_socketAddress; }

uint32_t Client::getSocketLength(void) const { return m_socketLength; }

bool Client::isAlive(void) const { return m_alive; }

bool Client::accept(int recipientFd) {
  bzero(&m_socketAddress, sizeof(t_sockaddr));
  m_fd = ::accept(recipientFd, &m_socketAddress, &m_socketLength);
  if (m_fd < 0) return false;
  m_alive = true;
  return true;
}

void Client::close(void) {
  ::close(m_fd);
  m_fd = -1;
  m_alive = false;
}

} /* namespace www */
