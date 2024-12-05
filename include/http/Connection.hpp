#pragma once

#include "http/ClientSocket.hpp"
#include "http/Request.hpp"

namespace http {

/**
 * @class Connection
 * @brief ...
 */
class Connection {
	public:
		Connection();
		Connection(int listen_socket) throw(std::exception);
		~Connection();
		Connection(const Connection& other);
		Connection& operator=(const Connection& rhs);

		const ClientSocket& getSocket(void) const;
		ClientSocket& getSocket(void);
		const http::Request& getRequest(void) const;
		http::Request& getRequest(void);
		void close(void);

		bool operator==(const Connection& other) const;
	private:
		ClientSocket m_client_socket;
		http::Request m_request;
};

}  // namespace http
