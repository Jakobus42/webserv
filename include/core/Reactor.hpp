#pragma once

#include "config/Parser.hpp"
#include "io/Dispatcher.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class VirtualServer;

	class Reactor : shared::mixin::NonCopyable {
		public:
			explicit Reactor(const config::Config& config);
			~Reactor();

			void run();

		private:
			static void handleSigint(int);

		private:
			typedef std::vector<VirtualServer*> vServers;

			config::Config m_config;
			io::Dispatcher m_dispatcher;
			vServers m_vServers;

			static bool m_isRunning;
	};

} /* namespace core */
