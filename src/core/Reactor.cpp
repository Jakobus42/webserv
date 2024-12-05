#include "core/Reactor.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include "core/RequestHandler.hpp"

#include <stdexcept>

namespace core {

	/**
	 * @brief Constructs a new Reactor object.
	 */
	Reactor::Reactor()
		: m_epoll_master_fd(-1)
		, m_virtual_servers() {
	}

	/**
	 * @brief Destroys the Reactor object.
	 * @todo Close all connections and http::VirtualServer sockets
	 */
	Reactor::~Reactor() {
		close(m_epoll_master_fd);
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Reactor object to copy.
	 */
	Reactor::Reactor(const Reactor &other)
		: m_epoll_master_fd(other.getEpollFd())
		, m_virtual_servers(other.getVirtualServers()) {
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
		m_virtual_servers = rhs.getVirtualServers();
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
		return m_virtual_servers;
	}

	std::vector<http::VirtualServer> &Reactor::getVirtualServers(void) {
		return m_virtual_servers;
	}

	void Reactor::addVirtualServer(config::t_server &serverConfig) throw(
		std::exception) {
		http::VirtualServer server(serverConfig);

		m_virtual_servers.push_back(server);
		if (m_virtual_servers.back().listen() == false)
			throw std::exception();
	}

	bool Reactor::removeVirtualServer(
		std::vector<http::VirtualServer>::iterator it) {
		m_virtual_servers.erase(it);
		return true;
	}

	void Reactor::registerHandler(int fd, core::RequestHandler *handler, uint32_t events) throw(std::runtime_error) {
		t_event event;

		event.data.fd = fd;
		event.events = events;
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
			throw std::runtime_error("Crud! Couldn't register handler!");
		}
		m_event_handlers[fd] = handler;
	}

	void Reactor::unregisterHandler(int fd) throw(std::runtime_error) {
		if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
			throw std::runtime_error("пиздец!");
		}
		m_event_handlers.erase(fd);
	}

	void Reactor::react() {
		t_event events[MAX_EVENTS];

		while (true) {
			int nEvents = epoll_wait(m_epoll_master_fd, events, MAX_EVENTS, 60);
			if (nEvents < 0) {
				throw std::runtime_error("Error: Epoll dun goofed up!");
			}

			for (int i = 0; i < nEvents; i++) {
				int fd = events[i].data.fd;
				uint32_t event_mask = events[i].events;
				if (m_event_handlers.find(fd) != m_event_handlers.end()) {
					m_event_handlers.at(fd)->handle(fd, event_mask);
				}
			}
		}
	}

	bool Reactor::addVirtualServers(config::t_config_data &configData) {
		for (size_t i = 0; i < configData.servers.size(); i++) {
			http::VirtualServer server(configData.servers.at(i));
			if (server.getSocket().init() == false) {
				m_virtual_servers.clear();
				std::cout << "Failed to initialize socket for server "
						  << configData.servers.at(i).server_names.at(0) << std::endl;
				return false;
			}
			for (size_t j = 0; j < configData.servers.at(i).locations.size(); j++) {
				// server.addLocation(configData.servers.at(i).locations.at(j));
			}
			m_virtual_servers.push_back(server);
		}
		return true;
	}

} /* namespace core */
