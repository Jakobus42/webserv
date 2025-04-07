#include "core/VirtualServer.hpp"

#include "shared/Logger.hpp"
#include "sstream"

namespace core {

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
			delete m_connections[i];
		}
	}

	Connection* VirtualServer::acceptNewConnection() {
		try {
			io::Socket* clientSocket = m_listenSocket.accept();
			if (!clientSocket) {
				return NULL;
			}

			Connection* conn = new Connection(clientSocket); //todo: maybe catch if pushback fails and delete?
			m_connections.push_back(conn);

			LOG_INFO("accepted new connection: " + conn->getConnectionInfo()); // todo
			return conn;
		} catch (const std::exception& e) {
			LOG_ERROR("failed to accept new connection" + std::string(e.what()));
		}
		return NULL;
	}

	void VirtualServer::listen() { m_listenSocket.listen(); }

	void VirtualServer::shutdown() { m_listenSocket.close(); }

	const io::Socket& VirtualServer::getListenSocket() const { return m_listenSocket; }

	std::string VirtualServer::getVirtualServerInfo() const {
		std::stringstream ss;
		ss << "(" << m_config.serverNames.front() << ":" << m_config.port << ")";
		return ss.str();
	}
} /* namespace core */
