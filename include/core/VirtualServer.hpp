#pragma once

#include "config/Parser.hpp"
#include "core/Connection.hpp"
#include "io/Socket.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class VirtualServer : shared::mixin::NonCopyable {
		public:
			explicit VirtualServer(const config::Config::ServerConfigs& configs);
			~VirtualServer();

			void listen();
			void shutdown();

			Connection* acceptNewConnection();
			void removeConnection(const Connection& conn);

			const io::Socket& getListenSocket() const;
			std::string getVirtualServerInfo() const;
			const std::vector<Connection*>& getActiveConnections() const;
			const config::Config::ServerConfigs& getServerConfigs() const;

		private:
			const config::Config::ServerConfigs& m_configs;

			io::Socket* m_listenSocket;
			std::vector<Connection*> m_connections;
	};

} /* namespace core */
