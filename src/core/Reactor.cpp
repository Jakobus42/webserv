#include "core/Reactor.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include "core/IOHandler.hpp"
#include "http/VirtualServer.hpp"

#include <cstring>
#include <stdexcept>

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

	bool Reactor::removeVirtualServer(
		std::vector<http::VirtualServer>::iterator it) {
		m_vServers.erase(it);
		return true;
	}

	void Reactor::registerHandler(int fd, IHandler *handler, uint32_t events) throw(std::runtime_error) {
		t_event event;

		event.data.fd = fd;
		event.events = events;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			std::cerr << "epoll_ctl failed for fd " << fd << ": " << strerror(errno) << std::endl;
			throw std::runtime_error("Crud! Couldn't register event handler!"); // TODO: dont use exception
		}
		m_eventHandlers[fd] = handler;
	}

	void Reactor::unregisterHandler(int fd) throw(std::runtime_error) {
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
			throw std::runtime_error("Пиздец! Couldn't unregister event handler!");
		}
		m_eventHandlers.erase(fd);
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];

		while (true) {
			for (std::vector<http::VirtualServer>::iterator it = m_vServers.begin(); it != m_vServers.end(); ++it) {
				http::VirtualServer &vServer = *it;
				bool isEstablished = true;

				while (isEstablished) {
					isEstablished = vServer.addConnection();
					if (isEstablished) {
						this->registerHandler(vServer.getConnections().back().getSocket().getFd(), new IOHandler(), EPOLLIN | EPOLLOUT);
					}
				}
			}
			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents < 0) {
				throw std::runtime_error("Epoll dun goofed up!");
			}
		}
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

} /* namespace core */
