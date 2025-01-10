#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "http/VirtualServer.hpp"

#include <csignal>
#include <cstring>
#include <stdexcept>

// TODO: abort connection if it gets dropped
// double check if there's another case where on success we shouldn't respond
// maybe when streaming several files for a single thingy, I guess
// if was out, change event to EPOLLIN
// unless Connection header was set to "close" in the Client's Request
// is there any other case?

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
		: m_epoll_master_fd(other.getEpollFd())
		, m_vServers(other.getVirtualServers())
		, m_eventHandlers(other.getEvents()) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Reactor object to assign from.
	 * @return A reference to the assigned Reactor object.
	 */
	Reactor& Reactor::operator=(const Reactor& rhs) {
		if (this == &rhs)
			return *this;
		m_epoll_master_fd = rhs.getEpollFd();
		m_vServers = rhs.getVirtualServers();
		return *this;
	}

	void Reactor::handleSigint(int signum) {
		if (signum == SIGINT) {
			m_reacting = false;
		}
	}

	void Reactor::init(void) throw(std::exception) {
		m_epoll_master_fd = epoll_create1(0);
		if (m_epoll_master_fd < 0) {
			throw std::runtime_error("epoll_create1() failed: " + std::string(strerror(errno)));
		}

		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);
	}

	void Reactor::addVirtualServer(config::t_server& serverConfig) throw(std::exception) {
		http::VirtualServer server(serverConfig);

		server.listen();
		m_vServers.push_back(server);
	}

	bool Reactor::addVirtualServers(config::t_config_data& configData) {
		for (size_t i = 0; i < configData.servers.size(); ++i) {
			try {
				http::VirtualServer server(configData.servers.at(i));
				server.getSocket().listen();
				m_vServers.push_back(server);
			} catch (const std::exception& e) {
				m_vServers.clear();
				std::cerr << e.what() << std::endl;
				return false;
			}
		}
		return true;
	}

	bool Reactor::removeVirtualServer(t_virtualServers::iterator it) {
		for (http::t_connections::iterator connection = it->getConnections().begin(); connection != it->getConnections().end(); ++connection) {
			connection->close();
		}
		it->getSocket().close();
		m_vServers.erase(it);
		return true;
	}

	void Reactor::deleteEventHandler(int fd) {
		try {
			m_eventHandlers.erase(fd);
		} catch (std::exception& e) {
			std::cout << "deleteEventHandler failed: " << e.what() << std::endl;
		}
	}

	void Reactor::registerHandler(http::VirtualServer& vServer, http::Connection& connection, uint32_t events) {
		t_event event;
		EventHandler handler(vServer, connection, events);
		int fd = connection.getClientSocketFd();

		event.events = events;
		event.data.fd = fd;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			throw std::runtime_error("Crud! Couldn't register event handler!");
		}
		m_eventHandlers.insert(std::make_pair(fd, handler));
	}

	void Reactor::unregisterHandler(int fd) throw(std::runtime_error) {
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
			std::cout << fd << " " << errno << std::endl;
			throw std::runtime_error("Пиздец! Couldn't unregister event handler!");
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
			t_event& event = events[i];
			EventHandler& handler = getEventHandler(event.data.fd);

			if (handler.shouldDrop()) {
				http::Connection& connection = handler.getConnection();
				int fd = connection.getClientSocketFd();
				std::cout << "Dropping connection for " << fd << "!" << std::endl;
				unregisterHandler(fd);
				handler.getServer().removeConnection(connection);
				deleteEventHandler(fd);
			}
		}
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];

		m_reacting = true;
		while (m_reacting) {
			acceptNewConnections();

			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents < 0 && m_reacting == true) {
				throw std::runtime_error("epoll_wait() failed: " + std::string(strerror(errno)));
			}

			handleEvents(events, nEvents);
			pruneConnections(events, nEvents);
		}
	}

	void Reactor::handleEvents(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event& event = events[i];
			EventHandler& handler = getEventHandler(event.data.fd);

			if (event.events & EPOLLHUP || event.events & EPOLLERR) {
				handler.killSelf();
			} else if (event.events & EPOLLIN) {
				handler.handleRequest();
				handler.buildResponse();
			} else if (event.events & EPOLLOUT) {
				handler.sendResponse();
			}
		}
	}

	EventHandler& Reactor::getEventHandler(int fd) {
		try {
			return m_eventHandlers.at(fd);
		} catch (std::exception& e) {
			std::cout << "getEventHandler failed: " << e.what() << std::endl;
			throw e;
		}
	}

	int Reactor::getEpollFd(void) const {
		return m_epoll_master_fd;
	}

	const t_virtualServers& Reactor::getVirtualServers(void) const {
		return m_vServers;
	}

	t_virtualServers& Reactor::getVirtualServers(void) {
		return m_vServers;
	}

	const std::map<int, EventHandler>& Reactor::getEvents(void) const {
		return m_eventHandlers;
	}

	std::map<int, EventHandler>& Reactor::getEvents(void) {
		return m_eventHandlers;
	}


} /* namespace core */
