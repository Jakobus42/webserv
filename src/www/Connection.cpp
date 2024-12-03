#include "../../include/www/Connection.hpp"

#include <unistd.h>

namespace www {

/**
 * @brief Constructs a new Connection object.
 */
Connection::Connection() : m_client_socket() {}

/**
 * @brief Constructs a new Connection object.
 */
Connection::Connection(int listen_socket) throw(std::exception) : m_client_socket(listen_socket) {}

/**
 * @brief Destroys the Connection object.
 * @note Closes the file, since this is our only reference to it.
 */
Connection::~Connection() { this->close(); }

/**
 * @brief Copy constructor.
 * @param other The other Connection object to copy.
 */
Connection::Connection(const Connection& other) : m_client_socket(other.getSocket()) {}

/**
 * @brief Copy assignment operator.
 * @param other The other Connection object to assign from.
 * @return A reference to the assigned Connection object.
 */
Connection& Connection::operator=(const Connection& rhs) {
  m_client_socket.operator=(rhs.getSocket());
  return *this;
}

bool Connection::operator==(const Connection& other) const { return other.getSocket() == this->getSocket(); }

void Connection::close(void) { this->getSocket().close(); }

const ClientSocket& Connection::getSocket(void) const { return m_client_socket; }

ClientSocket& Connection::getSocket(void) { return m_client_socket; }

} /* namespace www */
