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

		for (std::size_t i = 0; i < config.serverConfigs.size(); ++i) {
			const config::ServerConfig& serverConfig = config.serverConfigs[i];
			VirtualServer* vServer = new VirtualServer(serverConfig);

			m_vServers.push_back(vServer);
			vServer->listen();
			m_dispatcher.registerHandler(vServer->getListenSocket().getFd(), new AcceptHandler(vServer, m_dispatcher));
		}
	}

	Reactor::~Reactor() {
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
