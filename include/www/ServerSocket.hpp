#pragma once

#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr_in t_sockaddr_in;

namespace www {

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
  void close();

  void setBroken();

  int getFd() const;
  int getPort() const;
  t_sockaddr_in getSocketAddress() const;
  bool isOpen() const;
  bool isBound() const;
  bool isListening() const;

 private:
  int m_fd;
  t_sockaddr_in m_socketAddress;
  int m_port;
  bool m_open;
  bool m_bound;
  bool m_listening;
};

} /* namespace www */
