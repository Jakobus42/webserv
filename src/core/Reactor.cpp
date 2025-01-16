#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "core/AcceptHandler.hpp"
#include "http/VirtualServer.hpp"

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

	void Reactor::init(config::t_config_data& conf) {
		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);


		for (size_t i = 0; i < conf.servers.size(); ++i) {
			m_vServers.push_back(http::VirtualServer(conf.servers.at(i)));
			http::VirtualServer& vServer = m_vServers.back();
			vServer.init();

			IHandler* handler = new AcceptHandler(m_vServers.back(), m_dispatcher);
			m_dispatcher.registerHandler(vServer.getSocket(), EPOLLIN | EPOLLERR | EPOLLHUP, handler);
		}
	}

	void Reactor::react() {
		m_reacting = true;
		while (m_reacting) {
			m_dispatcher.dispatch();
		}
	}

	void Reactor::handleSigint(int signum) {
		if (signum == SIGINT) {
			m_reacting = false;
		}
	}

} /* namespace core */
