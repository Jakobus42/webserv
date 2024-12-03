#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "../configfile/Parser.hpp"
#include "VirtualServerSocket.hpp"

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
  VirtualServer(configfile::t_server& serverConfig);
  ~VirtualServer();
  VirtualServer(const VirtualServer& other);
  VirtualServer& operator=(const VirtualServer& other);

  const std::string& getName(void) const;
  uint64_t getMaxBodySize(void) const;
  const VirtualServerSocket& getSocket(void) const;
  VirtualServerSocket& getSocket(void);
  const t_connections& getConnections(void) const;

  bool addConnection(void);
  bool removeConnection(Connection& connection);

  bool listen(void);

 private:
  std::string m_name;
  uint64_t m_client_max_body_size;
  // Locations with metadata
  VirtualServerSocket m_listen_socket;
  t_connections m_connections;
};

} /* namespace www */
