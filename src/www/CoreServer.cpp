#include "../../include/www/CoreServer.hpp"

#include <sys/epoll.h>
#include <unistd.h>

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

bool CoreServer::addVirtualServer() {
  VirtualServer server;
  m_virtual_servers.push_back(server);
  return true;
}

bool CoreServer::removeVirtualServer() { return true; }

} /* namespace www */
