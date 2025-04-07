#pragma once

#include "io/Socket.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class Connection : shared::mixin::NonCopyable {
		public:
			explicit Connection(io::Socket* socket);
			~Connection();

			const io::Socket* getSocket() const;
			std::string getConnectionInfo() const;

		private:
			static const time_t DEFAULT_RECV_TIMEOUT = 30;
			static const time_t DEFAULT_SEND_TIMEOUT = 30;

			io::Socket* m_socket;
	};

} /* namespace core */
