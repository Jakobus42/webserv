#include "core/VirtualServer.hpp"

#include <unistd.h>

#include "shared/Logger.hpp"

#include <ctime>
#include <sstream>

namespace core {

	VirtualServer::VirtualServer(const config::Config::ServerConfigs& configs)
		: m_configs(configs)
		, m_listenSocket(NULL)
		, m_connections() {

#if defined(__linux__)
		m_listenSocket = new io::Socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
#else
		m_listenSocket = new io::Socket(AF_INET, SOCK_STREAM, 0);
		m_listenSocket->setNonBlocking(true);
#endif

		try {
			m_listenSocket->setReuseAddr(true);
			m_listenSocket->bind(m_configs.at(0).port, m_configs.at(0).ipAddress); // use default config because all have same port and adress just different name
		} catch (const std::exception&) {
			delete m_listenSocket;
			throw;
		}
	}

	VirtualServer::~VirtualServer() {
		shutdown();
		delete m_listenSocket;
	}

	Connection* VirtualServer::acceptNewConnection() {
		// this looks so ugly but we have to make sure to not leak...
		try {
			io::Socket* clientSocket = m_listenSocket->accept();
			if (!clientSocket) {
				return NULL;
			}

			Connection* conn = new Connection(clientSocket, m_configs.at(0).global.connectionTimeout);
			try {
				m_connections.push_back(conn);
			} catch (const std::exception&) {
				delete conn;
				throw;
			}

			LOG_INFO("accepted new connection: " + conn->getConnectionInfo());
			return conn;
		} catch (const std::bad_alloc&) {
			throw;
		} catch (const std::exception& e) {
			LOG_ERROR("failed to accept connection:" + std::string(e.what()));
		}
		return NULL;
	}

	void VirtualServer::removeConnection(const Connection& conn) {
		for (std::size_t i = 0; i < m_connections.size(); ++i) {
			if (m_connections[i]->getSocket().getFd() == conn.getSocket().getFd()) {
				delete m_connections[i];
				m_connections.erase(m_connections.begin() + i);
				return;
			}
		}
	}

	void VirtualServer::listen() {
		LOG_INFO("listening on: " + m_configs.front().socketAddress);
		m_listenSocket->listen();
	}

	void VirtualServer::shutdown() {
		if (m_listenSocket->isValid() == false) {
			return;
		}

		LOG_INFO("shutting down virtual server");

		for (std::size_t i = 0; i < m_connections.size(); ++i) {
			m_connections[i]->close();
			delete m_connections[i];
		}
		m_connections.clear();

		m_listenSocket->close();
	}

	const io::Socket& VirtualServer::getListenSocket() const { return *m_listenSocket; }

	std::string VirtualServer::getVirtualServerInfo() const {
		std::stringstream ss;

		ss << "(" << m_listenSocket->getLocalAddress()
		   << ":" << m_listenSocket->getLocalPort() << ")";
		return ss.str();
	}

	const std::vector<Connection*>& VirtualServer::getActiveConnections() const { return m_connections; }

	const config::Config::ServerConfigs& VirtualServer::getServerConfigs() const { return m_configs; }

} /* namespace core */
