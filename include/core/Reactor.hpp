#pragma once

#include <sys/epoll.h>

#include "core/AHandler.hpp"
#include "core/Dispatcher.hpp"
#include "core/VirtualServer.hpp"
#include "http/http.hpp"
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

			void init(const std::vector<config::Server>& serverConfigs);
			void react();

		private:
			static void handleSigint(int);

		private:
			static bool m_reacting;

			Dispatcher m_dispatcher;
			std::vector<http::VirtualServer> m_vServers;
	};

} // namespace core
