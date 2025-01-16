#pragma once

#include <sys/epoll.h>

#include "core/Dispatcher.hpp"
#include "core/IHandler.hpp"
#include "http/VirtualServer.hpp"
#include "http/constants.hpp"

#include <map>
#include <stdexcept>

namespace core {

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

		private:
			static bool m_reacting;

			Dispatcher m_dispatcher;
			std::vector<http::VirtualServer> m_vServers;
	};

} // namespace core
