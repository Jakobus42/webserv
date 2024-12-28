#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "core/RequestHandler.hpp"
#include "core/ResponseHandler.hpp"
#include "http/VirtualServer.hpp"

#include <cstring>
#include <stdexcept>

// TODO: refactor this fucker but shit is working (except some nice fd and memory leaks)

namespace core {

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
		close(m_epoll_master_fd);
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Reactor object to copy.
	 */
	Reactor::Reactor(const Reactor& other)
		: m_epoll_master_fd(other.getEpollFd())
		, m_vServers(other.getVirtualServers())
		, m_events(other.getEvents()) {
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

	int Reactor::getEpollFd(void) const {
		return m_epoll_master_fd;
	}

	void Reactor::init(void) throw(std::exception) {
		m_epoll_master_fd = epoll_create1(0);
		if (m_epoll_master_fd < 0)
			throw std::exception();
	}

	const t_virtualServers& Reactor::getVirtualServers(void) const {
		return m_vServers;
	}

	t_virtualServers& Reactor::getVirtualServers(void) {
		return m_vServers;
	}

	const std::map<int, EventData*>& Reactor::getEvents(void) const {
		return m_events;
	}

	std::map<int, EventData*>& Reactor::getEvents(void) {
		return m_events;
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

	bool Reactor::removeVirtualServer(
		t_virtualServers::iterator it) {
		m_vServers.erase(it);
		return true;
	}

	void Reactor::registerHandler(int fd, AHandler* handler, const HandlerContext& ctx, uint32_t events) {
		t_event event;

		event.events = events;
		EventData* data = new EventData(handler, ctx);
		m_events[fd] = data;
		event.data.ptr = m_events[fd];
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			delete static_cast<EventData*>(event.data.ptr);
			throw std::runtime_error("Crud! Couldn't register event handler!");
		}
	}

	void Reactor::modifyHandler(int fd, HandlerContext& ctx, uint32_t events) {
		t_event event;

		event.events = events;
		event.data.ptr = m_events[fd];
		ctx.events = events;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_MOD, fd, &event) < 0)
			throw std::runtime_error("Dang! Couldn't modify Handler!");
	}

	void Reactor::unregisterHandler(int fd) throw(std::runtime_error) {
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
			throw std::runtime_error("Пиздец! Couldn't unregister event handler!");
		}
		close(fd);
		std::cout << "Unregistered a handler!" << std::endl;
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];
		int prevEvents = 0;

		int i = 0;
		while (i < 10) {
			acceptNewConnections();

			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents < 0) {
				throw std::runtime_error("epoll_wait failed");
			}
			if (nEvents != prevEvents) {
				prevEvents = nEvents;
				std::cout << "Now handling " << nEvents << " events!" << std::endl;
				i++;
			}
			handleEvents(events, nEvents);
			// change event handlers if required
			pruneConnections(events, nEvents);
		}
		std::cout << "Handled 10 events (probably), stopping execution for now..." << std::endl;
	}

	void Reactor::pruneConnections(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event& event = events[i];
			EventData* data = static_cast<EventData*>(event.data.ptr);
			HandlerContext& ctx = data->ctx;

			if (data->handler->shouldDrop()) {
				std::cout << "Dropping connection!" << std::endl;
				ctx.vServer.removeConnection(ctx.conn);
				unregisterHandler(ctx.conn.getSocket().getFd());
				delete data->handler;
				delete data;
				continue;
			} else if (data->handler->completed()) {
				// std::cout << "Handler completed!" << std::endl;
				// TODO: only do this if we should actually drop the connection, otherwise keep alive
				ctx.vServer.removeConnection(ctx.conn);
				unregisterHandler(ctx.conn.getSocket().getFd());
				delete data->handler;
				delete data;
			} else if (data->handler->failed()) {
				std::cout << "Handler failed, taking it out back..." << std::endl;
				// ctx.vServer.removeConnection(ctx.conn);
				// unregisterHandler(ctx.conn.getSocket().getFd());
				// delete data->handler;
				// delete data;
			}
		}
	}

	void Reactor::acceptNewConnections() {
		for (t_virtualServers::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
			http::VirtualServer& vServer = *it;

			while (vServer.addConnection()) {
				http::Connection& conn = vServer.getConnections().back();
				AHandler* handler = NULL;

				try { // we have to get rid of this shit try stuff
					handler = new RequestHandler;
					this->registerHandler(conn.getClientSocketFd(), handler, HandlerContext(vServer, conn), EPOLLIN | EPOLLOUT);
				} catch (std::exception& e) {
					delete handler;
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}

	void Reactor::handleEvents(t_event* events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event& event = events[i];
			EventData* data = static_cast<EventData*>(event.data.ptr);
			AHandler& handler = *data->handler;
			HandlerContext& ctx = data->ctx;

			ctx.events = event.events;
			handler.handle(ctx);

			std::cout << "Handler state: " << handler.getState() << std::endl;
			if (handler.failed()) {
				std::cout << "HANDLEEVENTS FOUND FAILED HANDLER" << std::endl;
				// unregisterHandler(ctx.conn.getSocket().getFd());
				// ctx.vServer.removeConnection(ctx.conn);
				// delete data->handler;
				// delete data;
			} else if (handler.completed()) {
				std::cout << "HANDLEEVENTS FOUND COMPLETED" << std::endl;
				if (ctx.events == EPOLLIN) {
					// change event to EPOLLOUT
					// TODO: abort connection if it gets dropped
					// double check if there's another case where on success we shouldn't respond
					// maybe when streaming several files for a single thingy, I guess
					std::cout << "Modifying handler for " << ctx.conn.getSocket().getFd() << " to out woo wee" << std::endl;
					modifyHandler(ctx.conn.getSocket().getFd(), ctx, EPOLLOUT); // just pass ctx, dummy
					delete data->handler;
					data->handler = new ResponseHandler;
					// handler = *data->handler;
					continue;
				} else if (ctx.events == EPOLLOUT) {
					// change event to EPOLLIN
					// unless Connection header was set to "close" in the Client's Request
					// is there any other case?
					std::cout << "Modifying handler for " << ctx.conn.getSocket().getFd() << " to in woo wee" << std::endl;
					modifyHandler(ctx.conn.getSocket().getFd(), ctx, EPOLLIN);
					delete data->handler;
					data->handler = new RequestHandler;
					// handler = *data->handler;
					continue;
				}
				// unregisterHandler(ctx.conn.getSocket().getFd());
				// ctx.vServer.removeConnection(ctx.conn);
				// delete data->handler;
				// delete data;
			}
		}
	}

} /* namespace core */
