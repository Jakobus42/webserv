#include "core/ConnectionHandler.hpp"

#include "core/Connection.hpp"
#include "core/VirtualServer.hpp"

namespace core {

	// todo maybe make dispatcher global for ease of use in for example the cgi proccessor
	ConnectionHandler::ConnectionHandler(VirtualServer* vServer, Connection* conn, io::Dispatcher& dispatcher)
		: m_vServer(vServer)
		, m_connection(conn)
		, m_dispatcher(dispatcher) {
		(void)m_dispatcher;
	}

	ConnectionHandler::~ConnectionHandler() {
	}

	io::EventResult ConnectionHandler::onReadable(int32_t) {
		m_connection->updateActivityTimestamp();

		ssize_t bytesRead = 1;
		if (bytesRead == -1) {
			m_vServer->removeConnection(m_connection);
			return io::UNREGISTER;
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionHandler::onWriteable(int32_t) {
		m_connection->updateActivityTimestamp();
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionHandler::onError(int32_t) {
		m_vServer->removeConnection(m_connection);
		return io::UNREGISTER;
	}

} /* namespace core */
