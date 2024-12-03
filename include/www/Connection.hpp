#pragma once

#include "../www/ClientSocket.hpp"

namespace www {

/**
 * @class Connection
 * @brief ...
 */
class Connection {
 public:
  Connection();
  Connection(int listen_socket) throw(std::exception);
  ~Connection();
  Connection(const Connection& other);
  Connection& operator=(const Connection& rhs);

  const ClientSocket& getSocket(void) const;
  ClientSocket& getSocket(void);
  void close(void);

  bool operator==(const Connection& other) const;

 private:
  ClientSocket m_client_socket;
};

} /* namespace www */
