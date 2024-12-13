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
			const std::vector<http::VirtualServer>& getVirtualServers() const;
			std::vector<http::VirtualServer>& getVirtualServers();

			void addVirtualServer(config::t_server& serverConfig) throw(
				std::exception);
			bool removeVirtualServer(std::vector<http::VirtualServer>::iterator it);
			bool addVirtualServers(config::t_config_data& configData);

			void react();

		private:
			Reactor(const Reactor& other);
			Reactor& operator=(const Reactor& other);

			void registerHandler(int fd, AHandler* handler, const HandlerContext& ctx, uint32_t events = EPOLLIN);
			void unregisterHandler(int fd) throw(std::runtime_error);

			void acceptNewConnections();
			void handleEvents(t_event* events, int nEvents);

			int m_epoll_master_fd;
			std::vector<http::VirtualServer> m_vServers;
	};

} // namespace core
