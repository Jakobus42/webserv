#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include "defines.hpp"

#include <string>

namespace http {

	/**
	 * @class Socket
	 * @brief ...
	 */
	class Socket {
		public:
			Socket();
			~Socket();

			bool create();
			bool bind(uint16_t port, uint32_t ip = LOCALHOST_ADDRESS);
			bool listen(int backlog = SOMAXCONN);
			Socket accept(); // returns invalid socket on error
			void close();

			ssize_t send(const char* data, size_t len) const;
			ssize_t recv(char* buffer, size_t len) const;


			bool isValid() const;

			int getFd() const;
			struct sockaddr_in getSockaddr() const;

		private:
			int m_fd;
			struct sockaddr_in m_sockAddr;

			Socket(const Socket&) {}

			Socket& operator=(const Socket&) {}

			bool setNonBlocking();
	};

} /* namespace http */
