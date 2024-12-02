#pragma once

#include <netinet/in.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_MAX_EVENTS 10

namespace www {

typedef struct sockaddr_in t_sockaddr_in;
const uint32_t LOCALHOST_ADDRESS = 0x7F000001;   // 127.0.0.1
const uint32_t DEFAULT_MAX_BODY_SIZE = 1048576;  // 1MB

/**
 * @class ServerSocket
 * @brief ...
 */
class ServerSocket {
 public:
  // just make the constructoor throw, then wrap its creation
  ServerSocket();
  ServerSocket(int port);
  ~ServerSocket();
  ServerSocket(const ServerSocket& other);
  ServerSocket& operator=(const ServerSocket& other);

  bool init();
  bool create();
  bool bind();
  bool listen();
  bool createEpollThingy();
  void close();

  void setBroken();

  int getFd() const;
  int getEpollFd() const;
  int getPort() const;
  t_sockaddr_in getSocketAddress() const;
  bool isOpen() const;
  bool isBound() const;
  bool isListening() const;
  bool isEpollAlive() const;

 private:
  int m_fd;
  int m_epoll_fd;
  t_sockaddr_in m_socketAddress;
  int m_port;
  bool m_open;
  bool m_bound;
  bool m_listening;
  bool m_epoll_alive;
};

} /* namespace www */
