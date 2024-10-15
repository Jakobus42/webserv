#pragma once

#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <sys/socket.h>

#include <string>

namespace testing {

/**
 * @class MinimalWebserver
 * @brief A minimal webserver for testing purposes
 */
class MinimalWebserver {
 public:
  MinimalWebserver(std::string ip, int port);
  ~MinimalWebserver();

 private:
  std::string m_ip_address;
  int m_port;
  int m_socket;
  int m_new_socket;
  unsigned int m_socketAddress_len;
  struct sockaddr_in m_socketAddress;

  int start();
  void close();
  void startListen();
  void acceptConnection(int& new_socket);
};

} /* namespace testing */
