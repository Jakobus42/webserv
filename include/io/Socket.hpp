#pragma once

#include <netinet/in.h>

#include "shared/platform.hpp"

#include <cerrno>
#include <cstring>
#include <shared/NonCopyable.hpp>
#include <stdexcept>
#include <string>

namespace io {

	class Socket : shared::mixin::NonCopyable {
		public:
			explicit Socket(int domain, int type, int protocol);
			explicit Socket(int fd);
			~Socket();

			void bind(in_port_t port, uint32_t address = INADDR_ANY);
			void listen(int backlog = 64);
			Socket* accept();
			void shutdown(int mode = SHUT_RDWR);
			void close();

			ssize_t send(const void* buffer, size_t size, int flags = 0);
			ssize_t recv(void* buffer, size_t size, int flags = 0);

			template <typename T>
			void setSocketOption(int level, int optname, const T& optval) {
				if (!isValid()) {
					throw std::runtime_error("cannot set socket option: invalid socket");
				}

				if (setsockopt(m_fd, level, optname, &optval, sizeof(optval)) == -1) {
					throw std::runtime_error("setsockopt() failed: " + std::string(strerror(errno)));
				}
			}

			void setReuseAddr(bool enable);

#if ALLOW_FORBIDDEN_FUNCTIONS
			void setSocketFlag(int flag, bool enable);
#endif

			int getFd() const;
			std::string getPeerAddress() const;
			std::string getLocalAddress() const;
			in_port_t getPeerPort() const;
			in_port_t getLocalPort() const;

			bool isValid() const;

		private:
			std::string addrToString(const struct sockaddr_in& addr) const;

		private:
			int m_fd;
			struct sockaddr_in m_peerAddr;
			struct sockaddr_in m_localAddr;
	};

} /* namespace io */
