#pragma once

#include <netinet/in.h>

#include <shared/NonCopyable.hpp>
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

			void setReuseAddr(bool enable);
			void setNonBlocking(bool nonBlocking);

			int getFd() const;
			std::string getPeerAddress() const;
			in_port_t getPeerPort() const;

			bool isValid() const;

		private:
			int m_fd;
	};

} /* namespace io */
