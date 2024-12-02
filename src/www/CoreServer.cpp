#include "../../include/www/CoreServer.hpp"

#include <sys/epoll.h>
#include <unistd.h>

namespace www {

/**
 * @brief Constructs a new CoreServer object.
 */
CoreServer::CoreServer() : m_epoll_master_fd(-1) {}

/**
 * @brief Destroys the CoreServer object.
 */
CoreServer::~CoreServer() { close(m_epoll_master_fd); }

/**
 * @brief Copy constructor.
 * @param other The other CoreServer object to copy.
 */
CoreServer::CoreServer(const CoreServer&) {}

/**
 * @brief Copy assignment operator.
 * @param other The other CoreServer object to assign from.
 * @return A reference to the assigned CoreServer object.
 */
CoreServer& CoreServer::operator=(const CoreServer&) { return *this; }

int CoreServer::getEpollFd(void) const { return m_epoll_master_fd; }

void CoreServer::init(void) throw(std::exception) {
  m_epoll_master_fd = epoll_create1(0);
  if (m_epoll_master_fd < 0) throw std::exception();
}

// const std::vector<VirtualServer>& CoreServer::getVirtualServers(void) const { return m_virtual_servers; }
//     return m_virtual_servers;

} /* namespace www */
