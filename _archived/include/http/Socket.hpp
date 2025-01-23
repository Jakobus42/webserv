#pragma once

#include <netinet/in.h>
#include <sys/socket.h>

#include "http/http.hpp"

#include <string>

namespace http {

	typedef struct sockaddr_in t_sockaddr_in;

	/**
	 * @class Socket
	 * @brief ...
	 */
	class Socket {
		public:
			Socket();
			~Socket();

			Socket(const Socket&);
			Socket& operator=(const Socket&);

			void create();
			void bind(uint16_t port, uint32_t ip = LOCALHOST_ADDRESS);
			void listen(int backlog = SOMAXCONN);
			Socket accept();
			void close();

			ssize_t send(const char* data, size_t len) const;
			ssize_t recv(char* buffer, size_t len) const;

			bool isValid() const;

			int getFd() const;
			t_sockaddr_in getSockaddr() const;

		private:
			void setNonBlocking();

			int m_fd;
			t_sockaddr_in m_sockAddr;
	};

} /* namespace http */
