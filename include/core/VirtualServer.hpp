#pragma once

#include "config/ServerConfig.hpp"
#include "core/Connection.hpp"
#include "io/Socket.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class VirtualServer : shared::mixin::NonCopyable {
		public:
			VirtualServer(const config::ServerConfig& config = config::ServerConfig());
			~VirtualServer();

			void listen();
			void shutdown();
			Connection* acceptNewConnection();

			const io::Socket& getListenSocket() const;
			std::string getVirtualServerInfo() const;

		private:
			io::Socket m_listenSocket;
			std::vector<Connection*> m_connections;
			config::ServerConfig m_config;
	};

} /* namespace core */
