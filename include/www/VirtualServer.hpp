#pragma once

#include <stdint.h>

#include <string>

#include "ServerSocket.hpp"

#define ONE_MEGABYTE 1048576UL

namespace www {

/**
 * @class VirtualServer
 * @brief ...
 */
class VirtualServer {
 public:
  VirtualServer();
  ~VirtualServer();

  const std::string& getName(void) const;
  uint64_t getMaxBodySize(void) const;

 private:
  VirtualServer(const VirtualServer& other);
  VirtualServer& operator=(const VirtualServer& other);

  std::string m_name;
  uint64_t m_client_max_body_size;
  // Locations with metadata
  ServerSocket m_listen_socket;
  // Connections
};

} /* namespace www */
