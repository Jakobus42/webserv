#pragma once

#include <stdint.h>

#include "config/Parser.hpp"
#include "http/Connection.hpp"
#include "http/Socket.hpp"

#include <string>
#include <vector>

#define ONE_MEGABYTE 1048576UL
#define MAX_CONNECTIONS 500

namespace http {

	typedef std::vector<Connection> t_connections;

	/**
	 * @class VirtualServer
	 * @brief ...
	 */
	class VirtualServer {
		public:
			VirtualServer();
			VirtualServer(config::t_server& serverConfig);
			~VirtualServer();
			VirtualServer(const VirtualServer& other);
			VirtualServer& operator=(const VirtualServer& other);

			const std::vector<std::string>& getNames(void) const;
			const std::map<int, std::string>& getErrorPages(void) const;
			const std::vector<config::t_location>& getLocations(void) const;
			uint64_t getMaxBodySize(void) const;
			const Socket& getSocket(void) const;
			Socket& getSocket(void);
			const t_connections& getConnections(void) const;
			t_connections& getConnections(void);

			bool addConnection(void);
			bool removeConnection(Connection& connection);

			void listen(void);

		private:
			uint64_t m_client_max_body_size;
			std::vector<std::string> m_names;
			std::vector<config::t_location> m_locations;
			std::map<int, std::string> m_errorPages;
			Socket m_listen_socket;
			t_connections m_connections;
	};

} // namespace http
