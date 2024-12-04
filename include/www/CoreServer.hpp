#pragma once

#include <sys/epoll.h>

#include <map>
#include <queue>
#include <stdexcept>
#include <vector>

#include "runtime/EventHandler.hpp"
#include "www/VirtualServer.hpp"

namespace www {

typedef struct epoll_event t_event;

/**
 * @class CoreServer
 * @brief ...
 */
class CoreServer {
 public:
  CoreServer();
  ~CoreServer();

  void init(void) throw(std::exception);
  int getEpollFd() const;
  const std::vector<VirtualServer>& getVirtualServers() const;
  std::vector<VirtualServer>& getVirtualServers();

  void addVirtualServer() throw(std::exception);
  void addVirtualServer(const std::string& name, uint32_t port, uint64_t clientMaxBodySize) throw(std::exception);
  bool removeVirtualServer(std::vector<VirtualServer>::iterator it);

  void registerHandler(int fd, runtime::EventHandler* handler, uint32_t events = EPOLLIN) throw(std::runtime_error);
  void unregisterHandler(int fd) throw(std::runtime_error);
  void react();

 private:
  CoreServer(const CoreServer& other);
  CoreServer& operator=(const CoreServer& other);

  int m_epoll_master_fd;
  std::vector<VirtualServer> m_virtual_servers;
  std::map<int, runtime::EventHandler*> m_event_handlers;
};

} /* namespace www */
