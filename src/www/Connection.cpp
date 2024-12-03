#include "../../include/www/Connection.hpp"

#include <unistd.h>

namespace www {

/**
 * @brief Constructs a new Connection object.
 */
Connection::Connection() : m_client_socket() {}

/**
 * @brief Destroys the Connection object.
 */
Connection::~Connection() { ::close(m_client_socket.getFd()); }

/**
 * @brief Copy constructor.
 * @param other The other Connection object to copy.
 */
Connection::Connection(const Connection&) {}

/**
 * @brief Copy assignment operator.
 * @param other The other Connection object to assign from.
 * @return A reference to the assigned Connection object.
 */
Connection& Connection::operator=(const Connection&) { return *this; }

const ClientSocket& Connection::getSocket(void) const { return m_client_socket; }

} /* namespace www */
