#pragma once

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/Socket.hpp"

namespace http {

	/**
	 * @class Connection
	 * @brief ...
	 */
	class Connection {
		public:
			Connection();
			Connection(const Socket& socket);
			~Connection();
			Connection(const Connection& other);
			Connection& operator=(const Connection& rhs);

			const Socket& getSocket(void) const;
			Socket& getSocket(void);
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
			Socket m_clientSocket;
			http::Request m_requestBuffer;
			http::Response m_responseBuffer;
			char m_byteBuffer[BUFFER_SIZE];
	};

} // namespace http
