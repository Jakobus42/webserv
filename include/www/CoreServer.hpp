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

namespace www {

/**
 * @class CoreServer
 * @brief ...
 */
class CoreServer {
 public:
  CoreServer();
  ~CoreServer();

  void init() throw(std::exception);
  int getEpollFd() const;
  const std::vector<VirtualServer>& getVirtualServers() const;
  std::vector<VirtualServer>& getVirtualServers();

  void addVirtualServer(config::t_server& serverConfig) throw(std::exception);
  bool removeVirtualServer(std::vector<VirtualServer>::iterator it);
  bool addVirtualServers(config::t_config_data& configData);

  void registerHandler(int fd, runtime::RequestHandler* handler, uint32_t events = EPOLLIN) throw(std::runtime_error);
  void unregisterHandler(int fd) throw(std::runtime_error);
  void react();

 private:
  CoreServer(const CoreServer& other);
  CoreServer& operator=(const CoreServer& other);

  int m_epoll_master_fd;
  std::vector<VirtualServer> m_virtual_servers;
  std::map<int, runtime::RequestHandler*> m_event_handlers;
};

} /* namespace www */
