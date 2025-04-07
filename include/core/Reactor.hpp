#pragma once

#include "config/Parser.hpp"
#include "io/Dispatcher.hpp"
#include "shared/NonCopyable.hpp"

namespace core {

	class Reactor : shared::mixin::NonCopyable {
		public:
			Reactor(const config::Config& config);
			~Reactor();

			void run();

		private:
			static void handleSigint(int);

		private:
			config::Config m_config;
			io::Dispatcher m_dispatcher;

			static bool m_isRunning;
	};

} /* namespace core */
