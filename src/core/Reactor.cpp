#include "core/Reactor.hpp"

#include "core/AcceptHandler.hpp"
#include "shared/Logger.hpp"

#include <csignal>

namespace core {

	bool Reactor::m_isRunning = false;

	Reactor::Reactor(const config::Config& config)
		: m_config(config)
		, m_dispatcher() {
		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);

		m_dispatcher.registerHandler(-1, new AcceptHandler()); // -1 is the best server fd lelel
	}

	Reactor::~Reactor() {
	}

	void Reactor::run() {
		m_isRunning = true;

		// cppcheck-suppress knownConditionTrueFalse
		while (m_isRunning) {
			m_dispatcher.dispatch();
		}

		LOG_INFO("reactor shutting down...");
	}

	void Reactor::handleSigint(int signum) {
		if (signum == SIGINT) {
			m_isRunning = false;
		}
	}

} /* namespace core */
