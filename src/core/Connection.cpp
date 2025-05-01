#include "core/Connection.hpp"

#include "shared/Logger.hpp"

#include <ctime>
#include <sstream>

namespace core {

	const std::time_t Connection::DEFAULT_TIMEOUT = 60;

	// note: takes ownership of socket
	Connection::Connection(io::Socket* socket, std::time_t timeout)
		: m_socket(socket)
		, m_lastActivityTimestamp(std::time(NULL))
		, m_timeout(timeout)
		, m_isKeepAlive(true) {}

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

	bool Connection::hasTimedOut() const { return std::time(NULL) - m_lastActivityTimestamp > m_timeout; }

	void Connection::setIsKeepAlive(bool isKeepAlive) { m_isKeepAlive = isKeepAlive; }

	bool Connection::isKeepAlive() const { return m_isKeepAlive; }

	const io::Socket& Connection::getSocket() const { return *m_socket; }

	std::string Connection::getConnectionInfo() const {
		std::stringstream ss;
		ss << "(" << m_socket->getPeerAddress() << ":" << m_socket->getPeerPort() << ")";
		return ss.str();
	}

} /* namespace core */
