#pragma once

#include <sys/epoll.h>

#include "http/VirtualServer.hpp"
#include "shared/defines.hpp"

#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

namespace core {

	class AHandler;

	typedef std::vector<http::VirtualServer> t_virtualServers;

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
			const std::map<int, AHandler*>& getEvents() const;
			std::map<int, AHandler*>& getEvents();

			void addVirtualServer(config::t_server& serverConfig) throw(std::exception);
			bool removeVirtualServer(t_virtualServers::iterator it);
			bool addVirtualServers(config::t_config_data& configData);
			AHandler* upsertEventHandler(int fd, EPOLL_EVENTS event, struct HandlerContext& ctx);
			void removeEventHandler(int);
			AHandler* getEventHandler(int);

			void react();

		private:
			Reactor(const Reactor& other);
			Reactor& operator=(const Reactor& other);

			void registerHandler(int fd, AHandler* handler, EPOLL_EVENTS events);
			AHandler* modifyHandler(AHandler*, EPOLL_EVENTS events);
			void unregisterHandler(int fd) throw(std::runtime_error);
			void handleEvents(t_event* events, int nEvents);

			void acceptNewConnections();
			void pruneConnections(t_event* events, int nEvents);

		private:
			int m_epoll_master_fd;
			t_virtualServers m_vServers;
			std::map<int, AHandler*> m_eventHandlers;
	};

} // namespace core
