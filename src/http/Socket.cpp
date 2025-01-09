#include "http/Socket.hpp"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>

namespace http {

	/**
	 * @brief  Constructs a new Socket object.
	 */
	Socket::Socket()
		: m_fd(-1) {
		std::memset(&m_sockAddr, 0, sizeof(m_sockAddr));
	}

	/**
	 * @brief Closes the socket if open.
	 */
	Socket::~Socket() {
		this->close();
	}

	/**
	 * @brief Copies the socket by duplicating the file descriptor.
	 * @throws std::runtime_error if duplication fails.
	 */
	Socket::Socket(const Socket& other)
		: m_fd(-1)
		, m_sockAddr(other.m_sockAddr) {
		m_fd = dup(other.m_fd);
		if (m_fd == -1) {
			throw std::runtime_error("dup() failed: " + std::string(strerror(errno)));
		}
	}

	/**
	 * @brief Assigns another Socket object by duplicating its file descriptor.
	 * @throws std::runtime_error if duplication fails.
	 */
	Socket& Socket::operator=(const Socket& other) {
		if (this != &other) {
			this->close();

			m_fd = dup(other.m_fd);
			if (m_fd == -1) {
				throw std::runtime_error("dup() failed: " + std::string(strerror(errno)));
			}

			m_sockAddr = other.m_sockAddr;
		}
		return *this;
	}

	/**
	 * @brief Creates a TCP socket and sets it to non-blocking mode.
	 * @throws std::runtime_error on failure.
	 */
	void Socket::create() {
		m_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_fd == -1) {
			throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
		}

		int opt = true;
		if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			this->close();
			throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));
		}

		this->setNonBlocking();
	}

	/**
	 * @brief Binds the socket to a port and IP address.
	 * @throws std::runtime_error on failure.
	 */
	void Socket::bind(uint16_t port, uint32_t ip) {
		std::memset(&m_sockAddr, 0, sizeof(m_sockAddr));
		m_sockAddr.sin_family = AF_INET;
		m_sockAddr.sin_port = htons(port);
		m_sockAddr.sin_addr.s_addr = htonl(ip);

		if (::bind(m_fd, (struct sockaddr*)&m_sockAddr, sizeof(m_sockAddr)) < 0) {
			this->close();
			throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
		}
	}

	/**
	 * @brief Starts listening for connections.
	 * @throws std::runtime_error on failure.
	 */
	void Socket::listen(int backlog) {
		assert(backlog <= SOMAXCONN);

		if (::listen(m_fd, backlog) < 0) {
			this->close();
			throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
		}
	}

	/**
	 * @brief Accepts an incoming connection.
	 * @returns New Socket object for the client.
	 * @throws std::runtime_error on failure.
	 */
	Socket Socket::accept() {
		t_sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		std::memset(&clientAddr, 0, clientAddrLen);
		int clientFd = ::accept(m_fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if (clientFd == -1) {
			throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
		}

		Socket clientSocket;
		clientSocket.m_fd = clientFd;
		clientSocket.m_sockAddr = clientAddr;
		clientSocket.setNonBlocking();
		return clientSocket;
	}

	/**
	 * @brief Closes the socket.
	 * @throws std::runtime_error on failure.
	 */
	void Socket::close() {
		if (m_fd == -1) {
			return;
		}

		if (::close(m_fd) == -1) {
			throw std::runtime_error("close() failed: " + std::string(strerror(errno)));
		}
		m_fd = -1;
	}

	/**
	 * @brief Sends data through the socket.
	 * @returns The number of bytes sent or -1 on error.
	 */
	ssize_t Socket::send(const char* data, size_t len) const {
		return ::send(m_fd, data, len, 0);
	}

	/**
	 * @brief Receives data from the socket.
	 * @returns The number of bytes received or -1 on error.
	 */
	ssize_t Socket::recv(char* buffer, size_t len) const {
		return ::recv(m_fd, buffer, len, 0);
	}

	/**
	 * @brief Checks if the socket is valid.
	 * @returns True if valid, false otherwise.
	 */
	bool Socket::isValid() const {
		return m_fd >= 0;
	}

	/**
	 * @brief Gets the socket file descriptor.
	 * @returns The file descriptor.
	 */
	int Socket::getFd() const {
		return m_fd;
	}

	/**
	 * @brief Gets the socket address.
	 * @returns The sockaddr_in structure.
	 */
	t_sockaddr_in Socket::getSockaddr() const {
		return m_sockAddr;
	}

	/**
	 * @brief Sets the socket to non-blocking mode.
	 * @throws std::runtime_error on failure.
	 */
	void Socket::setNonBlocking() {
		if (fcntl(m_fd, F_SETFL, O_NONBLOCK)) {
			this->close();
			throw std::runtime_error("fcntl() failed to set non-blocking: " + std::string(strerror(errno)));
		}
	}

} /* namespace http */