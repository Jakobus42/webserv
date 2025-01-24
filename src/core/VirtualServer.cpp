#include "core/VirtualServer.hpp"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http/http.hpp"
#include "shared/stringUtils.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

namespace http {

	/**
	 * @brief Constructs a new VirtualServer object.
	 */
	VirtualServer::VirtualServer(config::t_server& conf)
		: m_config(conf)
		, m_clients()
		, m_listenSocket(-1)
		, m_logger() {
	}

	/**
	 * @brief Destroys the VirtualServer object.
	 * @todo Should prolly close everything down neatly, this happens only at exit.
	 * @warning Edit: Should NOT close everything, otherwise creating Sockets
	 * @warning and placing them in Reactor's vector triggers the destructor,
	 * @warning closing the socket in the process.
	 */
	VirtualServer::~VirtualServer() {
		if (m_listenSocket != -1) {
			this->shutDown();
		}
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other VirtualServer object to copy.
	 */
	VirtualServer::VirtualServer(const VirtualServer& other)
		: m_config(other.m_config)
		, m_clients(other.m_clients)
		, m_listenSocket(other.m_listenSocket) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other VirtualServer object to assign from.
	 * @return A reference to the assigned VirtualServer object.
	 */
	VirtualServer& VirtualServer::operator=(const VirtualServer& rhs) {
		if (this == &rhs)
			return *this;
		m_config = rhs.m_config;
		m_clients = rhs.m_clients;
		m_listenSocket = rhs.m_listenSocket;
		return *this;
	}

	void VirtualServer::init() {
		m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_listenSocket == -1) {
			throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
		}

		this->setNonBlocking(m_listenSocket);

		int32_t opt = true;
		if (setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));
		}

		struct sockaddr_in sockAddr;
		std::memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(m_config.port);
		sockAddr.sin_addr.s_addr = htonl(m_config.ip_address);
		if (::bind(m_listenSocket, reinterpret_cast<const sockaddr*>(&sockAddr), sizeof(sockAddr)) < 0) {
			throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
		}

		if (::listen(m_listenSocket, SOMAXCONN)) {
			throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
		}

		m_logger.setFile(m_config.server_names.at(0) + ".log"); // todo: why are there multiple names?!
	}

	int32_t VirtualServer::acceptClient() {
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		std::memset(&clientAddr, 0, clientAddrLen);
		int32_t clientSocket = ::accept(m_listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
		if (clientSocket == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return -1;
			}
			throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
		}

		this->setNonBlocking(clientSocket);
		m_clients.insert(std::make_pair(clientSocket, time(NULL)));

		this->log("accepted new client " + this->getClientInfo(clientSocket), shared::INFO);
		return clientSocket;
	}

	void VirtualServer::shutDown() {
		this->log("shutting down server", shared::INFO);
		for (std::map<int32_t, time_t>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
			int& fd = const_cast<int&>(it->first);
			this->close(&fd);
		}
		m_clients.clear();
		this->close(&m_listenSocket);
	}

	void VirtualServer::dropClient(int32_t clientSocket) {
		std::map<int32_t, time_t>::iterator it = m_clients.find(clientSocket);
		if (it != m_clients.end()) {
			this->log("dropping client " + this->getClientInfo(clientSocket), shared::INFO);

			m_clients.erase(it);
			this->close(&clientSocket);
		} else {
			this->log("Client socket " + this->getClientInfo(clientSocket) + " not found in the client map.", shared::WARNING);
		}
	}

	void VirtualServer::updateClientActivity(int32_t clientSocket) {
		this->log("updating client activity: " + this->getClientInfo(clientSocket), shared::INFO);
		m_clients.at(clientSocket) = time(NULL);
	}

	void VirtualServer::dropIdleClients() {
		time_t now = time(NULL);
		std::vector<int32_t> toDrop;

		for (std::map<int32_t, time_t>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
			if (now - it->second > CLIENT_TIMEOUT) {
				toDrop.push_back(it->first);
			}
		}

		for (std::vector<int32_t>::iterator it = toDrop.begin(); it != toDrop.end(); ++it) {
			dropClient(*it);
		}
	}

	void VirtualServer::log(const std::string& msg, shared::LogLevel level) {
		const std::string formatted = "[" + m_config.server_names.at(0) + "] " + msg;
		m_logger.log(formatted, level);
	}

	std::string VirtualServer::getClientInfo(int32_t clientSocket) const {
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		std::memset(&clientAddr, 0, clientAddrLen);
		if (getpeername(clientSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen) == -1) {
			return "Unknown client (socket: " + shared::string::to_string(clientSocket) + ")";
		}

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN); // todo idk if allowed

		int32_t clientPort = ntohs(clientAddr.sin_port);

		return "(IP: " + std::string(clientIP) + ", Port: " + shared::string::to_string(clientPort) + ")";
	}

	void VirtualServer::setNonBlocking(int32_t socket) {
		if (fcntl(socket, F_SETFL, O_NONBLOCK)) {
			throw std::runtime_error("fcntl() failed to set non-blocking: " + std::string(strerror(errno)));
		}
	}

	void VirtualServer::close(int32_t* fd) const {
		if (*fd != -1) {
			::close(*fd);
			*fd = -1;
		}
	}

	int32_t VirtualServer::getSocket(void) { return m_listenSocket; }

	const std::map<int32_t, time_t>& VirtualServer::getClients(void) const { return m_clients; }

	std::map<int32_t, time_t>& VirtualServer::getClients(void) { return m_clients; }

	const config::t_server& VirtualServer::getConfig() const { return m_config; }

} // namespace http
