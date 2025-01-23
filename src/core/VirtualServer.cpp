#include "core/VirtualServer.hpp"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http/constants.hpp"
#include "shared/stringUtils.hpp"

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

	bool VirtualServer::acceptClient() {
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		std::memset(&clientAddr, 0, clientAddrLen);
		int32_t clientSocket = ::accept(m_listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
		if (clientSocket == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return false;
			}
			throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
		}

		this->setNonBlocking(clientSocket);
		m_clients.push_back(clientSocket);

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN); // todo probably cant use this
		int32_t clientPort = ntohs(clientAddr.sin_port);
		std::string clientInfo = "(IP: " + std::string(clientIP) + ", Port: " + shared::string::to_string(clientPort) + ")";
		this->log("accepted new client " + clientInfo, shared::INFO);
		return true;
	}

	void VirtualServer::shutDown() {
		this->log("shutting down server", shared::INFO);
		for (std::vector<int32_t>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
			int32_t fd = *it;
			if (fd != -1) {
				::close(*it);
				*it = -1;
			}
		}

		if (m_listenSocket != -1) {
			::close(m_listenSocket);
			m_listenSocket = -1;
		}
	}

	void VirtualServer::log(const std::string& msg, shared::LogLevel level) {
		const std::string formatted = "[" + m_config.server_names.at(0) + "] " + msg;
		m_logger.log(formatted, level);
	}

	void VirtualServer::setNonBlocking(int32_t socket) {
		if (fcntl(socket, F_SETFL, O_NONBLOCK)) {
			throw std::runtime_error("fcntl() failed to set non-blocking: " + std::string(strerror(errno)));
		}
	}

	int32_t VirtualServer::getSocket(void) { return m_listenSocket; }

	const std::vector<int32_t>& VirtualServer::getClients(void) const { return m_clients; }

	std::vector<int32_t>& VirtualServer::getClients(void) { return m_clients; }

	const config::t_server& VirtualServer::getConfig() const { return m_config; }

} // namespace http
