#pragma once

#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

namespace www {

/**
 * @class Socket
 * @brief ...
 */
class Socket {
 public:
  // just make the constructoor throw, then wrap its creation
  Socket();
  Socket(int port);
  ~Socket();
  Socket(const Socket& other);
  Socket& operator=(const Socket& other);

  bool bind();
  bool init();
  void close();
  int getFd() const;
  int getPort() const;

 private:
  int m_fd;
  int m_port;
};

} /* namespace www */
