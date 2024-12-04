#include "www/CoreServer.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include <stdexcept>

namespace www {

/**
 * @brief Constructs a new CoreServer object.
 */
CoreServer::CoreServer() : m_epoll_master_fd(-1), m_virtual_servers() {}

/**
 * @brief Destroys the CoreServer object.
 */
CoreServer::~CoreServer() { close(m_epoll_master_fd); }

// /**
//  * @brief Copy constructor.
//  * @param other The other CoreServer object to copy.
//  */
// CoreServer::CoreServer(const CoreServer& other)
// 	: m_epoll_master_fd(other.getEpollFd()),
// 	  m_virtual_servers(other.getVirtualServers()) {
// }

// /**
//  * @brief Copy assignment operator.
//  * @param other The other CoreServer object to assign from.
//  * @return A reference to the assigned CoreServer object.
//  */
// CoreServer& CoreServer::operator=(const CoreServer& rhs) {
// 	if (this == &rhs)
// 		return *this;
// 	m_epoll_master_fd = rhs.getEpollFd();
// 	m_virtual_servers = rhs.getVirtualServers();
// 	return *this;
// }

int CoreServer::getEpollFd(void) const { return m_epoll_master_fd; }

void CoreServer::init(void) throw(std::exception) {
  m_epoll_master_fd = epoll_create1(0);
  if (m_epoll_master_fd < 0) throw std::exception();
}

const std::vector<VirtualServer>& CoreServer::getVirtualServers(void) const { return m_virtual_servers; }

std::vector<VirtualServer>& CoreServer::getVirtualServers(void) { return m_virtual_servers; }

void CoreServer::addVirtualServer() throw(std::exception) {
  VirtualServer server("Default Server", 80);

  m_virtual_servers.push_back(server);
  if (m_virtual_servers.back().listen() == false) throw std::exception();
}

void CoreServer::addVirtualServer(const std::string& name, uint32_t port,
                                  uint64_t clientMaxBodySize) throw(std::exception) {
  VirtualServer server(name, port, clientMaxBodySize);

  m_virtual_servers.push_back(server);
  if (m_virtual_servers.back().listen() == false) throw std::exception();
}

bool CoreServer::removeVirtualServer(std::vector<VirtualServer>::iterator it) {
  m_virtual_servers.erase(it);
  return true;
}

void CoreServer::registerHandler(int fd, runtime::EventHandler* handler, uint32_t events) throw(std::runtime_error) {
  t_event event;

  event.data.fd = fd;
  event.events = events;
  if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
    throw std::runtime_error("Crud! Couldn't register handler!");
  }
  m_event_handlers[fd] = handler;
}

void CoreServer::unregisterHandler(int fd) throw(std::runtime_error) {
  if (epoll_ctl(m_epoll_master_fd, EPOLL_CTL_DEL, fd, NULL) < 0) {
    throw std::runtime_error("пиздец!");
  }
  m_event_handlers.erase(fd);
}

void CoreServer::react() {
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

} /* namespace www */
