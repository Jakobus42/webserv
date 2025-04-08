#include "core/VirtualServer.hpp"

#include "ctime"
#include "shared/Logger.hpp"
#include "sstream"

namespace core {

	const time_t VirtualServer::CONNECTION_TIMEOUT = 120;

	VirtualServer::VirtualServer(const config::ServerConfig& config)
		: m_listenSocket()
		, m_connections()
		, m_config(config) {
		m_listenSocket.setNonBlocking(true);
		m_listenSocket.setReuseAddr(true);
		m_listenSocket.bind(m_config.port, m_config.ipAddress);
	}

	VirtualServer::~VirtualServer() {
		for (std::size_t i = 0; i < m_connections.size(); ++i) {
			m_connections[i]->close();
			delete m_connections[i];
		}
		shutdown();
	}

	Connection* VirtualServer::acceptNewConnection() {
		try {
			io::Socket* clientSocket = m_listenSocket.accept();
			if (!clientSocket) {
				return NULL;
			}

			Connection* conn = new Connection(clientSocket); // todo: maybe catch if pushback fails and delete?
			m_connections.push_back(conn);

			LOG_INFO("accepted new connection: " + conn->getConnectionInfo());
			return conn;
		} catch (const std::exception& e) {
			LOG_ERROR("failed to accept connection:" + std::string(e.what()));
		}
		return NULL;
	}

	void VirtualServer::removeConnection(Connection* conn) {
		for (std::size_t i = 0; i < m_connections.size(); ++i) {
			if (m_connections[i] == conn) {
				delete conn;
				m_connections.erase(m_connections.begin() + i);
				return;
			}
		}
	}

	void VirtualServer::removeInactiveConnections() {
		time_t now = std::time(NULL);
		for (std::size_t i = 0; i < m_connections.size(); ++i) {
			Connection* conn = m_connections[i];

			if (conn->getLastActivityTimestamp() == -1) {
				continue;
			}

			if (now - conn->getLastActivityTimestamp() > CONNECTION_TIMEOUT) { // todo: maybe make this configureable
				LOG_INFO("removing inactive connection: " + conn->getConnectionInfo());
				removeConnection(conn);
			}
		}
	}

	void VirtualServer::listen() { m_listenSocket.listen(); }

	void VirtualServer::shutdown() {
		LOG_INFO("shutting down virtual server");
		m_listenSocket.close();
	}

	const io::Socket& VirtualServer::getListenSocket() const { return m_listenSocket; }

	std::string VirtualServer::getVirtualServerInfo() const {
		std::stringstream ss;
		ss << "(name: " << m_config.serverNames.front() << "; port: " << m_config.port << ")";
		return ss.str();
	}
} /* namespace core */
