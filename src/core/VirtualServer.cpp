#include "core/VirtualServer.hpp"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http/constants.hpp"

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
		, m_listenSocket(-1) {
	}

	/**
	 * @brief Destroys the VirtualServer object.
	 * @todo Should prolly close everything down neatly, this happens only at exit.
	 * @warning Edit: Should NOT close everything, otherwise creating Sockets
	 * @warning and placing them in Reactor's vector triggers the destructor,
	 * @warning closing the socket in the process.
	 */
	VirtualServer::~VirtualServer() {
		for (std::vector<int32_t>::iterator it = m_clients.begin(); it != m_clients.end(); ++it) {
			int32_t fd = *it;
			if (fd != -1) {
				::close(*it);
			}
		}

		if (m_listenSocket != -1) {
			::close(m_listenSocket);
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
	}

	bool VirtualServer::acceptClient() {
		sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		std::memset(&clientAddr, 0, clientAddrLen);
		int clientSocket = ::accept(m_listenSocket, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
		if (clientSocket == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return false;
			}
			throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
		}

		this->setNonBlocking(clientSocket);
		m_clients.push_back(clientSocket);
		return true;
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
