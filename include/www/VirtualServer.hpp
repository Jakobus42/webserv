#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "ServerSocket.hpp"

#define ONE_MEGABYTE 1048576UL
#define MAX_CONNECTIONS 500

#include "../www/Connection.hpp"

namespace www {

typedef std::vector<Connection> t_connections;

/**
 * @class VirtualServer
 * @brief ...
 */
class VirtualServer {
 public:
  VirtualServer();
  VirtualServer(const std::string& name, int port = 80, uint64_t maxBodySize = ONE_MEGABYTE);
  ~VirtualServer();
  VirtualServer(const VirtualServer& other);
  VirtualServer& operator=(const VirtualServer& other);

  const std::string& getName(void) const;
  uint64_t getMaxBodySize(void) const;
  const ServerSocket& getSocket(void) const;
  ServerSocket& getSocket(void);
  const t_connections& getConnections(void) const;

  bool addConnection(void);
  bool removeConnection(Connection& connection);

  void listen(void);

 private:
  std::string m_name;
  uint64_t m_client_max_body_size;
  // Locations with metadata
  ServerSocket m_listen_socket;
  t_connections m_connections;
};

} /* namespace www */
