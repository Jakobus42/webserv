#pragma once

#include <vector>

#include "../../include/www/VirtualServer.hpp"

namespace www {

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

 private:
  CoreServer(const CoreServer& other);
  CoreServer& operator=(const CoreServer& other);

  int m_epoll_master_fd;
  std::vector<VirtualServer> m_virtual_servers;
};

} /* namespace www */
