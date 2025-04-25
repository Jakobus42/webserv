#include "core/Connection.hpp"

#include "shared/Logger.hpp"

#include <ctime>
#include <sstream>

namespace core {

	// note: takes ownership of socket
	Connection::Connection(io::Socket* socket)
		: m_socket(socket)
		, m_lastActivityTimestamp(-1)
		, m_isKeepAlive(true) {
		m_socket->setReuseAddr(true);
	}

	Connection::~Connection() {
		close();
		delete m_socket;
	}

	ssize_t Connection::recv(void* buffer, size_t size, int flags) { return m_socket->recv(buffer, size, flags); }

	ssize_t Connection::send(const void* buffer, size_t size, int flags) { return m_socket->send(buffer, size, flags); }

	void Connection::close() {
		LOG_INFO("closing connection");
		m_socket->close();
	}

	void Connection::updateActivityTimestamp() { m_lastActivityTimestamp = std::time(NULL); }

	time_t Connection::getLastActivityTimestamp() const { return m_lastActivityTimestamp; }

	void Connection::setIsKeepAlive(bool isKeepAlive) { m_isKeepAlive = isKeepAlive; }

	bool Connection::isKeepAlive() const { return m_isKeepAlive; }

	const io::Socket& Connection::getSocket() const { return *m_socket; }

	std::string Connection::getConnectionInfo() const {
		std::stringstream ss;
		ss << "(" << m_socket->getPeerAddress() << ":" << m_socket->getPeerPort() << ")";
		return ss.str();
	}

} /* namespace core */
