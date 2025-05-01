#pragma once

#include "io/Socket.hpp"
#include "shared/NonCopyable.hpp"

#include <ctime>

namespace core {

	class Connection : shared::mixin::NonCopyable {
		public:
			static const std::time_t DEFAULT_TIMEOUT;

		public:
			explicit Connection(io::Socket* socket, std::time_t timeout = DEFAULT_TIMEOUT);
			~Connection();

			ssize_t recv(void* buffer, size_t size, int flags = 0);
			ssize_t send(const void* buffer, size_t size, int flags = 0);
			void close();

			void updateActivityTimestamp();
			bool hasTimedOut() const;

			void setIsKeepAlive(bool isKeepAlive);
			bool isKeepAlive() const;

			const io::Socket& getSocket() const;
			std::string getConnectionInfo() const;

		private:
			io::Socket* m_socket;
			std::time_t m_lastActivityTimestamp;
			std::time_t m_timeout;
			bool m_isKeepAlive;
	};

} /* namespace core */
