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
			const http::Request& getRequestBuffer(void) const;
			http::Request& getRequestBuffer(void);
			const http::Response& getResponseBuffer(void) const;
			http::Response& getResponseBuffer(void);
			const char* getByteBuffer(void) const;
			char* getByteBuffer(void);

			void close(void);

			bool operator==(const Connection& other) const;

		private:
			ClientSocket m_clientSocket;
			http::Request m_requestBuffer;
			http::Response m_responseBuffer;
			char m_byteBuffer[BUFFER_SIZE];
	};

} // namespace http
