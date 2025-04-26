#include "io/Socket.hpp"

#include <arpa/inet.h>
#include <unistd.h>

#include "shared/stringUtils.hpp"

#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

namespace io {

	Socket::Socket(int domain, int type, int protocol)
		: m_fd(-1) {
		memset(&m_localAddr, 0, sizeof(m_localAddr));
		memset(&m_peerAddr, 0, sizeof(m_peerAddr));

		m_fd = socket(domain, type, protocol);
		if (m_fd == -1) {
			throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
		}
	}

	Socket::Socket(int fd)
		: m_fd(fd) {
		memset(&m_localAddr, 0, sizeof(m_localAddr));
		memset(&m_peerAddr, 0, sizeof(m_peerAddr));
	}

	Socket::~Socket() { close(); }

	void Socket::bind(in_port_t port, uint32_t address) {
		if (!isValid()) {
			throw std::runtime_error("cannot bind: invalid socket");
		}

		memset(&m_localAddr, 0, sizeof(m_localAddr));
		m_localAddr.sin_family = AF_INET;
		m_localAddr.sin_port = htons(port);
		m_localAddr.sin_addr.s_addr = htonl(address);

		if (::bind(m_fd, (struct sockaddr*)&m_localAddr, sizeof(m_localAddr)) == -1) {
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
		memset(&addr, 0, sizeof(addr));
		socklen_t addrLen = sizeof(addr);
		int connectionFd = ::accept(m_fd, (struct sockaddr*)&addr, &addrLen);
		if (connectionFd == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return NULL;
			} else {
				throw std::runtime_error("accept() failed: " + std::string(strerror(errno)));
			}
		}

		Socket* clientSocket = new Socket(connectionFd);
		clientSocket->m_peerAddr = addr;

		return clientSocket;
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

#if ALLOW_FORBIDDEN_FUNCTIONS
#	include <fcntl.h>

	void Socket::setSocketFlag(int flag, bool enable) {
		if (!isValid()) {
			throw std::runtime_error("cannot set socket flag: invalid socket");
		}

		int flags = fcntl(m_fd, F_GETFL, 0);
		if (flags == -1) {
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
		}

		if (enable) {
			flags |= flag;
		} else {
			flags &= ~flag;
		}

		if (fcntl(m_fd, F_SETFL, flags) == -1) {
			throw std::runtime_error("fcntl() failed: " + std::string(strerror(errno)));
		}
	}

#endif

	void Socket::setReuseAddr(bool enable) {
		int opt = enable ? 1 : 0;
		setSocketOption(SOL_SOCKET, SO_REUSEADDR, opt);
	}

	int Socket::getFd() const { return m_fd; }

	std::string Socket::getLocalAddress() const {
		if (!isValid()) {
			throw std::runtime_error("cannot get local: invalid socket");
		}

		return addrToString(m_localAddr);
	}

	in_port_t Socket::getLocalPort() const {
		if (!isValid()) {
			throw std::runtime_error("cannot get local: invalid socket");
		}

		return ntohs(m_localAddr.sin_port);
	}

	std::string Socket::getPeerAddress() const {
		if (!isValid()) {
			throw std::runtime_error("cannot get remote: invalid socket");
		}

		return addrToString(m_peerAddr);
	}

	in_port_t Socket::getPeerPort() const {
		if (!isValid()) {
			throw std::runtime_error("cannot get peer: invalid socket");
		}
		return ntohs(m_peerAddr.sin_port);
	}

	bool Socket::isValid() const { return m_fd != -1; }

	std::string Socket::addrToString(const struct sockaddr_in& addr) const {
		unsigned char* bytes = (unsigned char*)&addr.sin_addr.s_addr;

		return shared::string::toString((unsigned int)bytes[0]) + "." +
			shared::string::toString((unsigned int)bytes[1]) + "." +
			shared::string::toString((unsigned int)bytes[2]) + "." +
			shared::string::toString((unsigned int)bytes[3]);
	}
} /* namespace io */