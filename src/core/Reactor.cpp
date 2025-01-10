#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

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
		: m_epoll_master_fd(-1)
		, m_vServers()
		, m_eventHandlers() {
	}

	/**
	 * @brief Destroys the Reactor object.
	 * @todo Close all connections and http::VirtualServer sockets
	 */
	Reactor::~Reactor() {
		for (t_virtualServers::iterator server = m_vServers.begin(); server != m_vServers.end(); ++server) {
			for (http::t_connections::iterator connection = server->getConnections().begin(); connection != server->getConnections().end(); ++connection) {
				connection->close();
			}
			server->getSocket().close();
		}
		close(m_epoll_master_fd);
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Reactor object to copy.
	 */
	Reactor::Reactor(const Reactor& other)
		: m_epoll_master_fd(other.m_epoll_master_fd)
		, m_vServers(other.m_vServers)
		, m_eventHandlers(other.m_eventHandlers) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Reactor object to assign from.
	 * @return A reference to the assigned Reactor object.
	 */
	Reactor& Reactor::operator=(const Reactor& rhs) {
		if (this == &rhs) {
			return *this;
		}

		m_epoll_master_fd = rhs.m_epoll_master_fd;
		m_vServers = rhs.m_vServers;
		return *this;
	}

	void Reactor::init(config::t_config_data& conf) {
		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);

		m_epoll_master_fd = epoll_create1(0);
		if (m_epoll_master_fd < 0) {
			throw std::runtime_error("epoll_create1() failed: " + std::string(strerror(errno)));
		}

		for (size_t i = 0; i < conf.servers.size(); ++i) {
			http::VirtualServer server(conf.servers.at(i));
			server.listen();
			m_vServers.push_back(server);
		}
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];

		m_reacting = true;
		while (m_reacting) {
			acceptNewConnections();

			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents < 0) {
				if (errno == EINTR) {
					continue;
				}
				throw std::runtime_error("epoll_wait() failed: " + std::string(strerror(errno)));
			}

			handleEvents(events, nEvents);
			pruneConnections(events, nEvents);
		}
	}

	void Reactor::registerHandler(http::VirtualServer& vServer, http::Connection& connection, uint32_t events) {
		t_event event;
		EventHandler handler(vServer, connection, events);
		int fd = connection.getClientSocketFd();

		event.events = events;
		event.data.fd = fd;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			throw std::runtime_error("epoll_ctl() failed: " + std::string(strerror(errno)));
		}
		m_eventHandlers.insert(std::make_pair(fd, handler));
	}

	void Reactor::unregisterHandler(int fd) {
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
			throw std::runtime_error("epoll_ctl() failed: " + std::string(strerror(errno)));
		}
	}

	void Reactor::acceptNewConnections() {
		for (t_virtualServers::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
			http::VirtualServer& vServer = *it;

			while (vServer.addConnection()) {
				http::Connection& connection = vServer.getConnections().back();

				try {
					this->registerHandler(vServer, connection, EPOLLIN | EPOLLOUT | EPOLLHUP);
				} catch (std::exception& e) {
					std::cerr << "acceptNewConnections: " << e.what() << std::endl;
				}
			}
		}
	}

	void Reactor::pruneConnections(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			try {
				t_event& event = events[i];
				EventHandler& handler = m_eventHandlers.at(event.data.fd);

				if (handler.shouldDrop()) {
					http::Connection& connection = handler.getConnection();
					int fd = connection.getClientSocketFd();

					unregisterHandler(fd);
					handler.getServer().removeConnection(connection);
					m_eventHandlers.erase(fd);
				}
			} catch (const std::exception& e) {
				std::cerr << "pruneConnections: " << e.what() << std::endl;
			}
		}
	}

	void Reactor::handleEvents(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			try {
				t_event& event = events[i];
				EventHandler& handler = m_eventHandlers.at(event.data.fd);

				if (event.events & EPOLLHUP || event.events & EPOLLERR) {
					handler.killSelf();
				} else if (event.events & EPOLLIN) {
					handler.handleRequest();
					handler.buildResponse();
				} else if (event.events & EPOLLOUT) {
					handler.sendResponse();
				}
			} catch (const std::exception& e) {
				std::cerr << "handleEvents: " << e.what() << std::endl;
			}
		}
	}

	void Reactor::handleSigint(int signum) {
		if (signum == SIGINT) {
			m_reacting = false;
		}
	}

} /* namespace core */
