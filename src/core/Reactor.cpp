#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "http/VirtualServer.hpp"

#include <csignal>
#include <cstring>
#include <stdexcept>

// TODO: refactor this fucker but shit is working (except some nice fd and memory leaks)

namespace core {

	bool Reactor::m_reacting = true;

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
				connection->clientSocket.close();
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

	void Reactor::init(void) throw(std::exception) {
		m_epoll_master_fd = epoll_create1(0);
		if (m_epoll_master_fd < 0)
			throw std::exception();
	}

	//
	//	Getters and setters
	//

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
			connection->clientSocket.close();
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

	EventHandler& Reactor::getEventHandler(int fd) {
		try {
			return m_eventHandlers.at(fd);
		} catch (std::exception& e) {
			std::cout << "getEventHandler failed: " << e.what() << std::endl;
			throw e;
		}
	}

	void Reactor::registerHandler(http::VirtualServer& vServer, http::Connection& connection, uint32_t events) {
		EventHandler handler(vServer, connection, events);
		int fd = connection.clientSocket.getFd();
		t_event event;

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
		std::cout << "Unregistered a handler!" << std::endl;
	}

	void Reactor::acceptNewConnections() {
		for (t_virtualServers::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
			http::VirtualServer& vServer = *it;

			while (vServer.addConnection()) {
				http::Connection& connection = vServer.getConnections().back();

				try { // we have to get rid of this shit try stuff
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
				int fd = connection.clientSocket.getFd();
				std::cout << "Dropping connection for " << fd << "!" << std::endl;
				unregisterHandler(fd);
				handler.getServer().removeConnection(connection);
				deleteEventHandler(fd);
			}
		}
	}

	void Reactor::handleSigint(int signum) {
		if (signum == SIGINT)
			m_reacting = false;
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];
		int prevEvents = 0;
		int i = 0;

		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);

		while (m_reacting) {
			acceptNewConnections();
			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);

			if (nEvents != prevEvents && m_reacting == true) {
				std::cout << "Now handling " << nEvents << " instead of " << prevEvents << " events!" << std::endl;
				prevEvents = nEvents;
				i++;
			}

			if (nEvents < 0 && m_reacting == true) {
				std::cout << "Errno for epoll_wait: " << errno << std::endl;
				throw std::runtime_error("epoll_wait failed");
			}

			handleEvents(events, nEvents);
			pruneConnections(events, nEvents);
		}
	}

	void Reactor::handleEvents(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event& event = events[i];
			EventHandler& handler = getEventHandler(event.data.fd);

			if (event.events & EPOLLHUP) {
				// possibly also if epoll.events is ANYTHINHG else????????
				// this would indicate some kind of error, could this even happen since we don't listen to anything else?
				handler.killSelf();
				return;
			}
			if (event.events & EPOLLIN) {
				handler.handleRequest();
				handler.buildResponse();
				return;
			}
			if (event.events & EPOLLOUT) {
				handler.sendResponse();
				return;
			}
			// if (handler.completed() && !handler.shouldDrop()) {
			// 	std::cout << "HANDLEEVENTS FOUND COMPLETED HANDLER AND SHOULDN'T DROP, MOVE IT TO NEXT STATE" << std::endl;
			// 	// TODO: abort connection if it gets dropped
			// 	// double check if there's another case where on success we shouldn't respond
			// 	// maybe when streaming several files for a single thingy, I guess
			// 	// if was out, change event to EPOLLIN
			// 	// unless Connection header was set to "close" in the Client's Request
			// 	// is there any other case?
			// }
		}
	}

} /* namespace core */
