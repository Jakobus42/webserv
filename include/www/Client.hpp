#pragma once

#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr t_sockaddr;

namespace www {

/**
 * @class Client
 * @brief ...
 */
class Client {
 public:
  Client();
  ~Client();
  Client(const Client& other);
  Client& operator=(const Client& other);

  int getFd() const;
  struct sockaddr getSocketAddress() const;
  uint32_t getSocketLength() const;
  bool isAlive(void) const;

  bool accept(int recipientFd);
  void close(void);

 private:
  int m_fd;
  t_sockaddr m_socketAddress;
  uint32_t m_socketLength;
  bool m_alive;
};

} /* namespace www */
