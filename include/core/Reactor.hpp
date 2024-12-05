#pragma once

#include <sys/epoll.h>

#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

#include "config/Parser.hpp"
#include "runtime/RequestHandler.hpp"
#include "shared/defines.hpp"
#include "www/VirtualServer.hpp"

namespace core {

/**
 * @class Reactor
 * @brief ...
 */
class Reactor {
 public:
  Reactor();
  ~Reactor();

  void init() throw(std::exception);
  int getEpollFd() const;
  const std::vector<www::VirtualServer>& getVirtualServers() const;
  std::vector<www::VirtualServer>& getVirtualServers();

  void addVirtualServer(config::t_server& serverConfig) throw(std::exception);
  bool removeVirtualServer(std::vector<www::VirtualServer>::iterator it);
  bool addVirtualServers(config::t_config_data& configData);

  void registerHandler(int fd, runtime::RequestHandler* handler, uint32_t events = EPOLLIN) throw(std::runtime_error);
  void unregisterHandler(int fd) throw(std::runtime_error);
  void react();

 private:
  Reactor(const Reactor& other);
  Reactor& operator=(const Reactor& other);

  int m_epoll_master_fd;
  std::vector<www::VirtualServer> m_virtual_servers;
  std::map<int, runtime::RequestHandler*> m_event_handlers;
};

}  // namespace core
