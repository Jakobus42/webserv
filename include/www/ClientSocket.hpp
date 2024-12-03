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
  ClientSocket();
  ClientSocket(int listen_socket) throw(std::exception);
  ~ClientSocket();

  void accept(int listen_socket) throw(std::exception);

  int getFd() const;
  t_sockaddr getSocketAddress() const;
  bool isAlive(void) const;

 private:
  ClientSocket(const ClientSocket& other);
  ClientSocket& operator=(const ClientSocket& other);

  int m_fd;
  t_sockaddr m_socketAddress;
  uint32_t m_socketSize;
  int m_alive;
};

} /* namespace www */
