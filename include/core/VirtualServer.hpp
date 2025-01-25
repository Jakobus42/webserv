#pragma once

#include <stdint.h>

#include "config/Parser.hpp"
#include "shared/Logger.hpp"

#include <string>
#include <vector>

namespace http {

	/**
	 * @class VirtualServer
	 * @brief ...
	 */
	class VirtualServer {
		public:
			VirtualServer(config::t_server& conf);
			~VirtualServer();
			VirtualServer(const VirtualServer& other);
			VirtualServer& operator=(const VirtualServer& other);

			void init();
			int32_t acceptClient();
			void dropClient(int32_t clientSocket);
			void updateClientActivity(int32_t clientSocket);
			void dropIdleClients();
			void shutDown();

			void log(const std::string& msg, shared::LogLevel level = shared::DEBUG, int32_t clientSocket = -1);

			int32_t getSocket(void);
			const std::map<int32_t, time_t>& getClients(void) const;
			std::map<int32_t, time_t>& getClients(void);
			const config::t_server& getConfig() const;

		private:
			void setNonBlocking(int32_t socket);
			std::string getClientInfo(int32_t clientSocket) const;

			void close(int32_t* fd) const;

		private:
			static const time_t CLIENT_TIMEOUT = 3;

			config::t_server& m_config;
			std::map<int32_t, time_t> m_clients; // first is client fd and second is last activity/
			int32_t m_listenSocket;
			shared::Logger m_logger;
	};

} // namespace http
