#pragma once

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#include <exception>

#include "shared/defines.hpp"

namespace http {

typedef struct sockaddr t_sockaddr;

/**
 * @class ClientSocket
 * @brief ...
 */
class ClientSocket {
	public:
		ClientSocket(void);
		ClientSocket(int listen_socket) throw(std::exception);
		~ClientSocket(void);
		ClientSocket(const ClientSocket& other);
		ClientSocket& operator=(const ClientSocket& rhs);
		bool operator==(const ClientSocket& other) const;

		void accept(int listen_socket) throw(std::exception);

		int getFd(void) const;
		int getFd(void);
		t_sockaddr getSocketAddress(void) const;
		unsigned int getSocketSize(void) const;
		bool isAlive(void) const;
		void close(void);
	private:
		int m_fd;
		t_sockaddr m_socketAddress;
		unsigned int m_socketSize;
		int m_alive;
};

}  // namespace http
