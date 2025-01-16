#pragma once

#include <stdint.h>

#include "config/Parser.hpp"

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
			bool acceptClient();

			int32_t getSocket(void);
			const std::vector<int32_t>& getClients(void) const;
			std::vector<int32_t>& getClients(void);
			const config::t_server& getConfig() const;

		private:
			void setNonBlocking(int32_t socket);

		private:
			config::t_server& m_config;
			std::vector<int32_t> m_clients;
			int32_t m_listenSocket;
	};

} // namespace http
