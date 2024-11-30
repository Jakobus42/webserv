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

  bool init();
  bool create();
  bool bind();
  bool listen();
  void close();

  void setBroken();

  int getFd() const;
  int getPort() const;
  bool isOpen() const;
  bool isBound() const;
  bool isListening() const;

 private:
  int m_fd;
  int m_port;
  bool m_open;
  bool m_bound;
  bool m_listening;
};

} /* namespace www */
