#pragma once

#include <stdint.h>
#include <sys/socket.h>

#include <exception>

namespace www {

typedef struct sockaddr t_sockaddr;

/**
 * @class ClientSocket
 * @brief ...
 */
class ClientSocket {
 public:
  ClientSocket(void);
  ClientSocket(int listen_socket) throw(std::exception);
  ~ClientSocket(void);
  ClientSocket(const ClientSocket& other);
  ClientSocket& operator=(const ClientSocket& rhs);
  bool operator==(const ClientSocket& other) const;

  void accept(int listen_socket) throw(std::exception);

  int getFd(void) const;
  int getFd(void);
  t_sockaddr getSocketAddress(void) const;
  uint32_t getSocketSize(void) const;
  bool isAlive(void) const;
  void close(void);

 private:
  int m_fd;
  t_sockaddr m_socketAddress;
  uint32_t m_socketSize;
  int m_alive;
};

} /* namespace www */
