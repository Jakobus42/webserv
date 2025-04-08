#pragma once

#include "io/Socket.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class Connection : shared::mixin::NonCopyable {
		public:
			explicit Connection(io::Socket* socket);
			~Connection();

			ssize_t recv(void* buffer, size_t size, int flags = 0);
			ssize_t send(const void* buffer, size_t size, int flags = 0);
			void close();

			void updateActivityTimestamp();
			time_t getLastActivityTimestamp() const;

			void setIsKeepAlive(bool isKeepAlive);
			bool isKeepAlive() const;

			const io::Socket* getSocket() const;
			std::string getConnectionInfo() const;

		private:
			io::Socket* m_socket;
			time_t m_lastActivityTimestamp;
			bool m_isKeepAlive;
	};

} /* namespace core */
