#pragma once

#include <sys/epoll.h>

#include "core/EventHandler.hpp"
#include "http/VirtualServer.hpp"
#include "http/constants.hpp"

#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

#define MAX_EVENTS 256

namespace core {

	typedef std::vector<http::VirtualServer> t_virtualServers;
	typedef struct epoll_event t_event;

	/**
	 * @class Reactor
	 * @brief ...
	 */
	class Reactor {
		public:
			Reactor();
			~Reactor();

			void init(config::t_config_data& conf);
			void react();

		private:
			Reactor(const Reactor& other);
			Reactor& operator=(const Reactor& other);

			static void handleSigint(int);

			void registerHandler(http::VirtualServer& vServer, http::Connection& connection, uint32_t events);
			void unregisterHandler(int fd);
			void handleEvents(t_event* events, int nEvents);

			void acceptNewConnections();
			void pruneConnections(t_event* events, int nEvents);

		private:
			static bool m_reacting;
			int m_epoll_master_fd;

			t_virtualServers m_vServers;
			std::map<int, EventHandler> m_eventHandlers;
	};

} // namespace core
