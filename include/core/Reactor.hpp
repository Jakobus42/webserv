#pragma once

#include <sys/epoll.h>

#include "core/Dispatcher.hpp"
#include "core/IHandler.hpp"
#include "http/VirtualServer.hpp"
#include "http/constants.hpp"
#include "shared/NonCopyable.hpp"

#include <map>
#include <stdexcept>

namespace core {

	/**
	 * @class Reactor
	 * @brief ...
	 */
	class Reactor : shared::NonCopyable {
		public:
			Reactor();
			~Reactor();

			void init(config::t_config_data& conf);
			void react();

		private:
			static void handleSigint(int);

		private:
			static bool m_reacting;

			Dispatcher m_dispatcher;
			std::vector<http::VirtualServer> m_vServers;
	};

} // namespace core
