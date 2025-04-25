#include "io/Socket.hpp"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <stdexcept>

namespace io {

	Socket::Socket(int domain, int type, int protocol)
		: m_fd(-1) {

		m_fd = socket(domain, type, protocol);
		if (m_fd == -1) {
			throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
		}
	}

	Socket::Socket(int fd)
		: m_fd(fd) {}

	Socket::~Socket() { close(); }

	void Socket::bind(in_port_t port, uint32_t address) {
		if (!isValid()) {
			throw std::runtime_error("cannot bind: invalid socket");
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(address);

		if (::bind(m_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
			throw std::runtime_error("bind() failed: " + std::string(strerror(errno)));
		}
	}

	void Socket::listen(int backlog) {
		if (!isValid()) {
			throw std::runtime_error("cannot listen: invalid socket");
		}

		if (::listen(m_fd, backlog) == -1) {
			throw std::runtime_error("listen() failed: " + std::string(strerror(errno)));
		}
	}

	Socket* Socket::accept() {
		if (!isValid()) {
			throw std::runtime_error("cannot accept: invalid socket");
		}

		struct sockaddr_in addr;
		socklen_t addrLen = sizeof(addr);

		int connectionFd = ::accept(m_fd, (struct sockaddr*)&addr, &addrLen);
		if (connectionFd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return NULL;
			} else {
				throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
			}
		}
		return new Socket(connectionFd);
	}

	void Socket::shutdown(int mode) {
		if (!isValid()) {
			return;
		}

		if (::shutdown(m_fd, mode) == -1) {
			throw std::runtime_error("shutdown() failed: " + std::string(strerror(errno)));
		}
	}

	void Socket::close() {
		if (!isValid()) {
			return;
		}

		::close(m_fd);
		m_fd = -1;
	}

	ssize_t Socket::send(const void* buffer, size_t size, int flags) {
		if (!isValid()) {
			throw std::runtime_error("cannot send on invalid socket");
		}

		return ::send(m_fd, buffer, size, flags);
	}

	ssize_t Socket::recv(void* buffer, size_t size, int flags) {
		if (!isValid()) {
			throw std::runtime_error("cannot recv on invalid socket");
		}

		return ::recv(m_fd, buffer, size, flags);
	}

	void Socket::setNonBlocking(bool nonBlocking) {
		if (!isValid()) {
			throw std::runtime_error("cannot set non-blocking: invalid socket");
		}

		int flags = fcntl(m_fd, F_GETFL, 0);
		if (flags == -1) {
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
		}

		if (nonBlocking) {
			flags |= O_NONBLOCK;
		} else {
			flags &= ~O_NONBLOCK;
		}

		if (fcntl(m_fd, F_SETFL, flags) == -1) {
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
		}
	}

	void Socket::setReuseAddr(bool enable) {
		if (!isValid()) {
			throw std::runtime_error("cannot set option: invalid socket");
		}

		int opt = enable;
		if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));
		}
	}

	int Socket::getFd() const { return m_fd; }

	std::string Socket::getPeerAddress() const {
		if (!isValid()) {
			throw std::runtime_error("cannot get peer: invalid socket");
		}

		struct sockaddr_in addr;
		socklen_t addrLen = sizeof(addr);

		if (getpeername(m_fd, (struct sockaddr*)&addr, &addrLen) == -1) {
			throw std::runtime_error("getpeername() failed: " + std::string(strerror(errno)));
		}

		char addrStr[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &addr.sin_addr, addrStr, sizeof(addrStr)) == NULL) {
			throw std::runtime_error("inet_ntop() failed: " + std::string(strerror(errno)));
		}

		return std::string(addrStr);
	}

	in_port_t Socket::getPeerPort() const {
		if (!isValid()) {
			throw std::runtime_error("cannot get peer: invalid socket");
		}

		struct sockaddr_in addr;
		socklen_t addrLen = sizeof(addr);

		if (getpeername(m_fd, (struct sockaddr*)&addr, &addrLen) == -1) {
			throw std::runtime_error("getpeername() failed: " + std::string(strerror(errno)));
		}

		return ntohs(addr.sin_port);
	}

	bool Socket::isValid() const { return m_fd != -1; }

} /* namespace io */