#include "http/Connection.hpp"

#include <unistd.h>

namespace http {

/**
 * @brief Constructs a new Connection object.
 */
Connection::Connection() : m_client_socket(), m_request() {
}

/**
 * @brief Constructs a new Connection object.
 */
Connection::Connection(int listen_socket) throw(std::exception)
	: m_client_socket(listen_socket), m_request() {
}

/**
 * @brief Destroys the Connection object.
 * @note Closes the file, since this is our only reference to it.
 */
Connection::~Connection() {
	this->close();
}

/**
 * @brief Copy constructor.
 * @param other The other Connection object to copy.
 */
Connection::Connection(const Connection& other)
	: m_client_socket(other.getSocket()), m_request(other.getRequest()) {
}

/**
 * @brief Copy assignment operator.
 * @param other The other Connection object to assign from.
 * @return A reference to the assigned Connection object.
 */
Connection& Connection::operator=(const Connection& rhs) {
	if (this == &rhs)
		return *this;
	m_client_socket.operator=(rhs.getSocket());
	m_request = rhs.getRequest();
	return *this;
}

bool Connection::operator==(const Connection& other) const {
	return other.getSocket() == this->getSocket();
}

void Connection::close(void) {
	this->getSocket().close();
}

const ClientSocket& Connection::getSocket(void) const {
	return m_client_socket;
}

ClientSocket& Connection::getSocket(void) {
	return m_client_socket;
}

const http::Request& Connection::getRequest(void) const {
	return m_request;
}

}  // namespace http
