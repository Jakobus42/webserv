#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "core/AcceptHandler.hpp"
#include "core/VirtualServer.hpp"

#include <csignal>
#include <cstring>
#include <stdexcept>

namespace core {

	bool Reactor::m_reacting = false;

	/**
	 * @brief Constructs a new Reactor object.
	 */
	Reactor::Reactor()
		: m_dispatcher()
		, m_vServers() {
	}

	/**
	 * @brief Destroys the Reactor object.
	 * @todo Close all connections and http::VirtualServer sockets
	 */
	Reactor::~Reactor() {
	}

	void Reactor::init(const std::vector<config::ServerConfig>& serverConfigs) {
		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);


		for (size_t i = 0; i < serverConfigs.size(); ++i) {
			m_vServers.push_back(http::VirtualServer(serverConfigs.at(i)));
			http::VirtualServer& vServer = m_vServers.back();
			vServer.init();

			AHandler* handler = new AcceptHandler(vServer, m_dispatcher);
			m_dispatcher.registerHandler(vServer.getSocket(), EPOLLIN, handler);
		}
	}

	void Reactor::react() {
		m_reacting = true;
		// cppcheck-suppress knownConditionTrueFalse
		while (m_reacting) {
			m_dispatcher.dispatch();
			for (std::vector<http::VirtualServer>::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
				http::VirtualServer& vServer = *it;
				vServer.dropIdleClients();
			}
		}
	}

	void Reactor::handleSigint(int signum) {
		if (signum == SIGINT) {
			m_reacting = false;
		}
	}

} /* namespace core */
