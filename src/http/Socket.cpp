#include "http/Socket.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>

namespace http {

	Socket::Socket()
		: m_fd(-1) {
		std::memset(&m_sockAddr, 0, sizeof(m_sockAddr));
	}

	Socket::~Socket() {
		this->close();
	}

	bool Socket::create() {
		m_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (m_fd == -1)
			return false;

		int opt = true;
		if (this->setNonBlocking() == false ||
			setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			this->close();
			return false;
		}
		return true;
	}

	bool Socket::bind(uint16_t port, uint32_t ip) {
		std::memset(&m_sockAddr, 0, sizeof(m_sockAddr));
		m_sockAddr.sin_family = AF_INET;
		m_sockAddr.sin_port = htons(port);
		m_sockAddr.sin_addr.s_addr = htonl(ip);

		if (::bind(m_fd, (struct sockaddr*)&m_sockAddr, sizeof(m_sockAddr)) < 0) {
			this->close();
			return false;
		}
		return true;
	}

	bool Socket::listen(int backlog) {
		if (::listen(m_fd, backlog) < 0) {
			this->close();
			return false;
		}
		return true;
	}

	Socket Socket::accept() {
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);

		std::memset(&clientAddr, 0, clientAddrLen);
		int clientFd = ::accept(m_fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if (clientFd == -1) {
			return Socket();
		}

		if (this->setNonBlocking() == false) {
			::close(clientFd);
			return Socket();
		}

		Socket clientSocket;
		clientSocket.m_fd = clientFd;
		clientSocket.m_sockAddr = clientAddr;
		return clientSocket;
	}

	void Socket::close() {
		::close(m_fd);
		m_fd = -1;
	}

	ssize_t Socket::send(const char* data, size_t len) const {
		return ::send(m_fd, data, len, 0);
	}

	ssize_t Socket::recv(char* buffer, size_t len) const {
		return ::recv(m_fd, buffer, len, 0);
	}

	bool Socket::isValid() const {
		return m_fd >= 0;
	}

	int Socket::getFd() const {
		return m_fd;
	}

	struct sockaddr_in Socket::getSockaddr() const {
		return m_sockAddr;
	}

	bool Socket::setNonBlocking() {
		return fcntl(m_fd, F_SETFL, O_NONBLOCK) == -1 ? false : true;
	}

} /* namespace http */
