#include "core/Reactor.hpp"

#include "core/AcceptEventHandler.hpp"
#include "shared/Logger.hpp"

#include <csignal>
#include <iostream>

namespace core {

	bool Reactor::m_isRunning = false;

	Reactor::Reactor()
		: m_dispatcher()
		, m_vServers() {
	}

	void Reactor::init(const config::Config& config) {
		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);

		for (std::size_t i = 0; i < config.listenServerConfigs.size(); ++i) {
			VirtualServer* vServer = new VirtualServer(config.listenServerConfigs[i]);

			try {
				m_vServers.push_back(vServer);
			} catch (const std::exception&) {
				delete vServer;
				throw;
			}

			vServer->listen();
			m_dispatcher.registerHandler(vServer->getListenSocket().getFd(), new AcceptEventHandler(*vServer, m_dispatcher));
		}
	}

	Reactor::~Reactor() {
		m_dispatcher.cleanup();
		for (std::size_t i = 0; i < m_vServers.size(); ++i) {
			delete m_vServers[i];
		}
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
