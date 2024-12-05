#pragma once

#include <netinet/in.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "shared/defines.hpp"

#define DEFAULT_MAX_EVENTS 256
#define MAX_EVENTS 256

namespace www {

typedef struct sockaddr_in t_sockaddr_in;
typedef struct sockaddr t_sockaddr;

/**
 * @class VirtualServerSocket
 * @brief ...
 */
class VirtualServerSocket {
 public:
  // just make the constructoor throw, then wrap its creation
  VirtualServerSocket();
  VirtualServerSocket(int port, uint32_t ip = LOCALHOST_ADDRESS);
  ~VirtualServerSocket();
  VirtualServerSocket(const VirtualServerSocket& other);
  VirtualServerSocket& operator=(const VirtualServerSocket& other);

  bool init();
  bool create();
  bool bind();
  bool listen();
  void close();

  void setBroken();

  int getFd() const;
  uint32_t getIp() const;
  int getPort() const;
  t_sockaddr_in getSocketAddress() const;
  bool isOpen() const;
  bool isBound() const;
  bool isListening() const;

 private:
  int m_fd;
  uint32_t m_ip;
  int m_port;
  t_sockaddr_in m_socketAddress;
  bool m_open;
  bool m_bound;
  bool m_listening;
};

} /* namespace www */
