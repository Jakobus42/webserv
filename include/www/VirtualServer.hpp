#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "VirtualServerSocket.hpp"
#include "config/Parser.hpp"
#include "www/Connection.hpp"

#define ONE_MEGABYTE 1048576UL
#define MAX_CONNECTIONS 500

namespace www {

typedef std::vector<Connection> t_connections;

/**
 * @class VirtualServer
 * @brief ...
 */
class VirtualServer {
 public:
  VirtualServer();
  VirtualServer(config::t_server& serverConfig);
  ~VirtualServer();
  VirtualServer(const VirtualServer& other);
  VirtualServer& operator=(const VirtualServer& other);

  const std::vector<std::string>& getNames(void) const;
  const std::map<int, std::string>& getErrorPages(void) const;
  const std::vector<config::t_location>& getLocations(void) const;
  uint64_t getMaxBodySize(void) const;
  const VirtualServerSocket& getSocket(void) const;
  VirtualServerSocket& getSocket(void);
  const t_connections& getConnections(void) const;

  bool addConnection(void);
  bool removeConnection(Connection& connection);

  bool listen(void);

 private:
  uint64_t m_client_max_body_size;
  std::vector<std::string> m_names;
  std::vector<config::t_location> m_locations;
  std::map<int, std::string> m_errorPages;
  VirtualServerSocket m_listen_socket;
  t_connections m_connections;
};

} /* namespace www */
