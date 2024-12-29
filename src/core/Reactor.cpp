#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "core/AHandler.hpp"
#include "core/RequestHandler.hpp"
#include "core/ResponseHandler.hpp"
#include "http/VirtualServer.hpp"

#include <csignal>
#include <cstring>
#include <stdexcept>

// TODO: refactor this fucker but shit is working (except some nice fd and memory leaks)

namespace core {

	static bool g_running = true;

	/**
	 * @brief Constructs a new Reactor object.
	 */
	Reactor::Reactor()
		: m_epoll_master_fd(-1)
		, m_vServers() {
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
		for (std::map<int, AHandler*>::iterator it = m_eventHandlers.begin(); it != m_eventHandlers.end(); ++it) {
			delete it->second;
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

	const std::map<int, AHandler*>& Reactor::getEvents(void) const {
		return m_eventHandlers;
	}

	std::map<int, AHandler*>& Reactor::getEvents(void) {
		return m_eventHandlers;
	}

	void Reactor::addVirtualServer(config::t_server& serverConfig) throw(std::exception) {
		http::VirtualServer server(serverConfig);

		if (server.listen() == false)
			throw std::runtime_error("A VirtualServer couldn't acquire its socket!");

		m_vServers.push_back(server);
	}

	bool Reactor::addVirtualServers(config::t_config_data& configData) {
		for (size_t i = 0; i < configData.servers.size(); ++i) {
			http::VirtualServer server(configData.servers.at(i));
			if (server.getSocket().init() == false) {
				m_vServers.clear();
				std::cout << "Failed to initialize socket for server "
						  << configData.servers.at(i).server_names.at(0) << std::endl;
				return false;
			}
			for (size_t j = 0; j < configData.servers.at(i).locations.size(); ++j) {
				// server.addLocation(configData.servers.at(i).locations.at(j));
			}
			m_vServers.push_back(server);
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

	void Reactor::acceptNewConnections() {
		for (t_virtualServers::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
			http::VirtualServer& vServer = *it;

			while (vServer.addConnection()) {
				http::Connection& conn = vServer.getConnections().back();
				AHandler* handler = NULL;

				try { // we have to get rid of this shit try stuff
					handler = new RequestHandler(HandlerContext(vServer, conn));
					this->registerHandler(conn.getClientSocketFd(), handler, EPOLLIN);
				} catch (std::exception& e) {
					delete handler;
					std::cerr << "acceptNewConnections: " << e.what() << std::endl;
				}
			}
		}
	}

	void Reactor::pruneConnections(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event& event = events[i];
			AHandler* handler = getEventHandler(event.data.fd);
			// AHandler* handler = m_eventHandlers.at(event.)

			if (handler != NULL && handler->shouldDrop()) {
				const HandlerContext ctx = handler->getContext();
				int fd = ctx.conn.getClientSocketFd();
				std::cout << "Dropping connection!" << std::endl;
				unregisterHandler(fd);
				ctx.vServer.removeConnection(ctx.conn);
			}
		}
	}

	// if found, replaces an existing handler for the same fd
	// allocates a new AHandler
	AHandler* Reactor::upsertEventHandler(int fd, EPOLL_EVENTS event, HandlerContext& ctx) {
		AHandler* handler = NULL;
		try {
			ctx.events = event;

			switch (event) {
				case EPOLLIN:
					handler = new RequestHandler(ctx);
					break;
				case EPOLLOUT:
					handler = new ResponseHandler(ctx);
					break;
				default:
					std::cout << "Notice: couldn't add event handler, event " << event << " is not implemented." << std::endl;
					return NULL;
			}
			handler->getContext().events = event;
			if (m_eventHandlers.find(fd) != m_eventHandlers.end()) {
				removeEventHandler(fd);
			}
			m_eventHandlers[fd] = handler;
			return handler;
		} catch (std::exception& e) {
			std::cout << "upsertEventHandler failed: " << e.what() << std::endl;
			delete handler;
			return NULL;
		}
	}

	// deallocates an AHandler if found
	void Reactor::removeEventHandler(int fd) {
		try {
			delete m_eventHandlers.at(fd);
			m_eventHandlers.erase(fd);
		} catch (std::exception& e) {
			std::cout << "removeEventHandler failed: " << e.what() << std::endl;
		}
	}

	AHandler* Reactor::getEventHandler(int fd) {
		try {
			// return m_eventHandlers.at(fd); // lol
			AHandler* h = m_eventHandlers.at(fd);
			return h;
		} catch (std::exception& e) {
			std::cout << "getEventHandler failed: " << e.what() << std::endl;
			return NULL;
		}
	}

	void Reactor::registerHandler(int fd, AHandler* handler, EPOLL_EVENTS events) {
		t_event event;

		event.events = events;
		event.data.fd = fd;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			throw std::runtime_error("Crud! Couldn't register event handler!");
		}
		m_eventHandlers[fd] = handler;
	}

	AHandler* Reactor::modifyHandler(AHandler* oldHandler, EPOLL_EVENTS events) {
		HandlerContext& ctx = oldHandler->getContext();
		int fd = ctx.conn.getClientSocketFd();
		t_event event;

		event.events = events;
		event.data.fd = fd;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_MOD, fd, &event) < 0) {
			throw std::runtime_error("Dang! Couldn't modify Handler!");
		}

		AHandler* newHandler = upsertEventHandler(fd, events, ctx);
		if (newHandler == NULL) {
			throw std::runtime_error("Argh! Couldn't modify handler, upsert failed!");
		}
		return newHandler;
	}

	void Reactor::unregisterHandler(int fd) throw(std::runtime_error) {
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
			std::cout << fd << " " << errno << std::endl;
			throw std::runtime_error("Пиздец! Couldn't unregister event handler!");
		}
		removeEventHandler(fd);
		std::cout << "Unregistered a handler!" << std::endl;
	}

	void sigint_handler(int signum) {
		if (signum == SIGINT)
			g_running = false;
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];
		int prevEvents = 0;

		int i = 0;
		signal(SIGINT, sigint_handler);
		while (g_running) {
			acceptNewConnections();

			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents != prevEvents) {
				std::cout << "Now handling " << nEvents << " instead of " << prevEvents << " events!" << std::endl;
				prevEvents = nEvents;
				i++;
			}
			if (nEvents < 0 && g_running == true) {
				std::cout << "Errno for epoll_wait: " << errno << std::endl;
				throw std::runtime_error("epoll_wait failed");
			}
			handleEvents(events, nEvents);
			// change event handlers if required
			// updateEventHandlers();
			pruneConnections(events, nEvents);
		}
		std::cout << "Handled 10 events (probably), stopping execution for now..." << std::endl;
	}

	void Reactor::handleEvents(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event& event = events[i];
			AHandler* handler = getEventHandler(event.data.fd);
			if (handler == NULL) {
				std::cout << "Notice: handleEvents couldn't reinterpret handler ptr" << std::endl;
				continue;
			}

			HandlerContext& ctx = handler->getContext();
			ctx.events = static_cast<EPOLL_EVENTS>(event.events);
			handler->handle();

			if (handler->completed() && !handler->shouldDrop()) {
				std::cout << "HANDLEEVENTS FOUND HANDLER TO REPLACE" << std::endl;
				EPOLL_EVENTS newEvents = ctx.events == EPOLLIN ? EPOLLOUT : EPOLLIN;
				// TODO: abort connection if it gets dropped
				// double check if there's another case where on success we shouldn't respond
				// maybe when streaming several files for a single thingy, I guess
				// if was out, change event to EPOLLIN
				// unless Connection header was set to "close" in the Client's Request
				// is there any other case?
				handler = modifyHandler(handler, newEvents); // just pass ctx, dummy
				std::cout << "Modified handler for " << ctx.conn.getSocket().getFd() << " to event " << newEvents << std::endl;
			} else {
				std::cout << "HANDLER " << handler->getContext().conn.getSocket().getFd() << " IN PROGRESS OR SOMETHING, MAY ALSO BE DONE" << std::endl;
			}
		}
		std::cout << "Handled events" << std::endl;
	}

} /* namespace core */
