#pragma once

#include "http/ClientSocket.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"

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
			int getClientSocketFd(void) const;
			const http::Request& getRequest(void) const;
			http::Request& getRequest(void);
			const http::Response& getResponse(void) const;
			http::Response& getResponse(void);
			const char* getBuffer(void) const;
			char* getBuffer(void);

			void close(void);

			bool operator==(const Connection& other) const;

		private:
			ClientSocket m_client_socket;
			http::Request m_request;
			http::Response m_response;
			char m_read_buffer[BUFFER_SIZE];
	};

} // namespace http
