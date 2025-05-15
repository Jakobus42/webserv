#pragma once

#include "config/Config.hpp"
#include "io/Dispatcher.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class VirtualServer;

	class Reactor : shared::mixin::NonCopyable {
		public:
			Reactor();
			~Reactor();

			void init(const config::Config& config);
			void run();

		private:
			static void handleSigint(int);

		private:
			typedef std::vector<VirtualServer*> vServers;

			io::Dispatcher m_dispatcher;
			vServers m_vServers;

			static bool m_isRunning;
	};

} /* namespace core */
