#include "core/Connection.hpp"

#include <sstream>

namespace core {

	// note: takes ownership of socket
	Connection::Connection(io::Socket* socket)
		: m_socket(socket) {
		m_socket->setReuseAddr(true);
		m_socket->setNonBlocking(true);
		m_socket->setReceiveTimeout(DEFAULT_RECV_TIMEOUT, 0);
		m_socket->setSendTimeout(DEFAULT_SEND_TIMEOUT, 0);
	}

	Connection::~Connection() { delete m_socket; }

	const io::Socket* Connection::getSocket() const { return m_socket; }

	std::string Connection::getConnectionInfo() const {
		std::stringstream ss;
		ss << "(" << m_socket->getPeerAddress() << ":" << m_socket->getPeerPort() << ")";
		return ss.str();
	}

} /* namespace core */
