#include "core/Reactor.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "core/IOHandler.hpp"
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
		for (std::vector<http::VirtualServer>::iterator server = m_vServers.begin(); server != m_vServers.end(); ++server) {
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
	Reactor::Reactor(const Reactor &other)
		: m_epoll_master_fd(other.getEpollFd())
		, m_vServers(other.getVirtualServers()) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Reactor object to assign from.
	 * @return A reference to the assigned Reactor object.
	 */
	Reactor &Reactor::operator=(const Reactor &rhs) {
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

	const std::vector<http::VirtualServer> &Reactor::getVirtualServers(void) const {
		return m_vServers;
	}

	std::vector<http::VirtualServer> &Reactor::getVirtualServers(void) {
		return m_vServers;
	}

	void Reactor::addVirtualServer(config::t_server &serverConfig) throw(
		std::exception) {
		http::VirtualServer server(serverConfig);
		if (server.listen() == false)
			throw std::runtime_error("A VirtualServer couldn't acquire its socket!");

		m_vServers.push_back(server);
	}

	bool Reactor::addVirtualServers(config::t_config_data &configData) {
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
		std::vector<http::VirtualServer>::iterator it) {
		m_vServers.erase(it);
		return true;
	}

	void Reactor::registerHandler(int fd, AHandler *handler, const HandlerContext &ctx, uint32_t events) {
		t_event event;

		event.events = events;
		event.data.ptr = new EventData(handler, ctx);
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			delete static_cast<EventData *>(event.data.ptr);
			throw std::runtime_error("Crud! Couldn't register event handler!");
		}
	}

	void Reactor::unregisterHandler(int fd) throw(std::runtime_error) {
		t_event event;

		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, &event) < 0) {
			throw std::runtime_error("Пиздец! Couldn't unregister event handler!");
		}
		close(fd);
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];

		while (true) {
			acceptNewConnections();

			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents < 0) {
				throw std::runtime_error("epoll_wait failed");
			}

			handleEvents(events, nEvents);
		}
	}

	void Reactor::acceptNewConnections() {
		for (std::vector<http::VirtualServer>::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
			http::VirtualServer &vServer = *it;

			while (vServer.addConnection()) {
				http::Connection &conn = vServer.getConnections().back();
				AHandler *handler = NULL;

				try {
					handler = new IOHandler;
					this->registerHandler(conn.getSocket().getFd(), handler, HandlerContext(vServer, conn), EPOLLIN | EPOLLOUT);
				} catch (std::exception &e) {
					delete handler;
				}
			}
		}
	}

	void Reactor::handleEvents(t_event *events, int nEvents) {
		for (int i = 0; i < nEvents; ++i) {
			t_event &event = events[i];
			EventData *data = static_cast<EventData *>(event.data.ptr);
			HandlerContext &ctx = data->ctx;

			ctx.events = event.events;
			data->handler->handle(ctx);

			if (data->handler->hasCompleted()) {
				unregisterHandler(ctx.conn.getSocket().getFd());
				delete data->handler;
				delete data;
			}
		}
	}

} /* namespace core */
