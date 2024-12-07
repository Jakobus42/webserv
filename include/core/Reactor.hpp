#pragma once

#include <sys/epoll.h>

#include "core/RequestHandler.hpp"
#include "http/VirtualServer.hpp"
#include "shared/defines.hpp"

#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

namespace core {

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
			const std::vector<http::VirtualServer> &getVirtualServers() const;
			std::vector<http::VirtualServer> &getVirtualServers();

			void addVirtualServer(config::t_server &serverConfig) throw(
				std::exception);
			bool removeVirtualServer(std::vector<http::VirtualServer>::iterator it);
			bool addVirtualServers(config::t_config_data &configData);

			void registerHandler(
				int fd, core::RequestHandler *handler, uint32_t events = EPOLLIN) throw(std::runtime_error);
			void unregisterHandler(int fd) throw(std::runtime_error);
			void react();
		private:
			Reactor(const Reactor &other);
			Reactor &operator=(const Reactor &other);

			int m_epoll_master_fd;
			std::vector<http::VirtualServer> m_virtual_servers;
			std::map<int, core::RequestHandler *> m_event_handlers;
	};

} // namespace core
