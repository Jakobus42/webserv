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

			void init() throw(std::exception);
			int getEpollFd() const;
			const t_virtualServers& getVirtualServers() const;
			t_virtualServers& getVirtualServers();
			const std::map<int, EventHandler>& getEvents() const;
			std::map<int, EventHandler>& getEvents();

			void addVirtualServer(config::t_server& serverConfig) throw(std::exception);
			bool removeVirtualServer(t_virtualServers::iterator it);
			bool addVirtualServers(config::t_config_data& configData);
			void deleteEventHandler(int fd);
			EventHandler& getEventHandler(int fd);

			void react();

		private:
			Reactor(const Reactor& other);
			Reactor& operator=(const Reactor& other);

			static void handleSigint(int);

			void registerHandler(http::VirtualServer& vServer, http::Connection& connection, uint32_t events);
			void unregisterHandler(int fd) throw(std::runtime_error);
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
