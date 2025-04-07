#pragma once

#include <netinet/in.h>

#include <shared/NonCopyable.hpp>
#include <string>

namespace io {

	class Socket : shared::mixin::NonCopyable {
		public:
			enum ShutdownMode {
				SHUTDOWN_READ = SHUT_RD,
				SHUTDOWN_WRITE = SHUT_WR,
				SHUTDOWN_BOTH = SHUT_RDWR
			};

			Socket();
			explicit Socket(int fd);
			~Socket();

			void bind(in_port_t port, uint32_t address = INADDR_ANY);
			void listen(int backlog = 64);
			Socket* accept();
			void shutdown(ShutdownMode mode = SHUTDOWN_BOTH);
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
