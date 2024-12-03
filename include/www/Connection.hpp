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
  ~Connection();
  Connection(const Connection& other);
  Connection& operator=(const Connection& other);

  const ClientSocket& getSocket(void) const;

 private:
  ClientSocket m_client_socket;
};

} /* namespace www */
