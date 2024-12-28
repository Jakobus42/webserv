#pragma once

#include <sys/epoll.h>

#include "core/AHandler.hpp"
#include "http/VirtualServer.hpp"
#include "shared/defines.hpp"

#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

namespace core {

	typedef std::vector<http::VirtualServer> t_virtualServers;

	struct EventData {
			AHandler* handler;
			HandlerContext ctx;

			EventData(AHandler* handler, const HandlerContext& ctx)
				: handler(handler)
				, ctx(ctx) {}
	};

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
			const std::map<int, EventData*>& getEvents() const;
			std::map<int, EventData*>& getEvents();

			void addVirtualServer(config::t_server& serverConfig) throw(std::exception);
			bool removeVirtualServer(t_virtualServers::iterator it);
			bool addVirtualServers(config::t_config_data& configData);

			void react();

		private:
			Reactor(const Reactor& other);
			Reactor& operator=(const Reactor& other);

			void registerHandler(int fd, AHandler* handler, const HandlerContext& ctx, uint32_t events = EPOLLIN);
			void modifyHandler(int fd, HandlerContext& ctx, uint32_t events);
			void unregisterHandler(int fd) throw(std::runtime_error);
			void handleEvents(t_event* events, int nEvents);

			void acceptNewConnections();
			void pruneConnections(t_event* events, int nEvents);

		private:
			int m_epoll_master_fd;
			t_virtualServers m_vServers;
			std::map<int, EventData*> m_events;
	};

} // namespace core
