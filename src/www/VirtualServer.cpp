#include "../../include/www/VirtualServer.hpp"

#include <algorithm>
#include <iostream>

namespace www {

/**
 * @brief Constructs a new VirtualServer object.
 */
VirtualServer::VirtualServer()
    : m_name("Default Server"), m_client_max_body_size(ONE_MEGABYTE), m_listen_socket(80), m_connections() {}

/**
 * @brief Constructs a new VirtualServer object.
 */
VirtualServer::VirtualServer(const std::string& name, int port, uint64_t maxBodySize)
    : m_name(name), m_client_max_body_size(maxBodySize), m_listen_socket(port), m_connections() {}

/**
 * @brief Destroys the VirtualServer object.
 * @todo Should prolly close everything down neatly, this happens only at exit.
 */
VirtualServer::~VirtualServer() {
  for (t_connections::iterator it = m_connections.begin(); it != m_connections.end(); ++it) {
    it->close();
  }
  this->getSocket().close();
}

/**
 * @brief Copy constructor.
 * @param other The other VirtualServer object to copy.
 */
VirtualServer::VirtualServer(const VirtualServer& other)
    : m_name(other.getName()),
      m_client_max_body_size(other.getMaxBodySize()),
      m_listen_socket(other.getSocket()),
      m_connections(other.getConnections()) {}

/**
 * @brief Copy assignment operator.
 * @param other The other VirtualServer object to assign from.
 * @return A reference to the assigned VirtualServer object.
 */
VirtualServer& VirtualServer::operator=(const VirtualServer& rhs) {
  if (this == &rhs) return *this;
  m_name = rhs.getName();
  m_client_max_body_size = rhs.getMaxBodySize();
  m_listen_socket = rhs.getSocket();
  m_connections = rhs.getConnections();
  return *this;
}

const std::string& VirtualServer::getName(void) const { return m_name; }

uint64_t VirtualServer::getMaxBodySize(void) const { return m_client_max_body_size; }

const ServerSocket& VirtualServer::getSocket(void) const { return m_listen_socket; }

ServerSocket& VirtualServer::getSocket(void) { return m_listen_socket; }

const t_connections& VirtualServer::getConnections(void) const { return m_connections; }

/**
 * @brief Accepts the next incoming connection in the VirtualServerSocket's
 * queue
 *
 * @return true if the connection could be established
 * @return false if accept() within Connection->ClientSocket fails
 */
bool VirtualServer::addConnection(void) {
  try {
    Connection newConnection(m_listen_socket.getFd());
    m_connections.push_back(newConnection);
    std::cout << "Added a connection!" << std::endl;
  } catch (std::exception& e) {
    std::cerr << "Exception caught in VServer: " << e.what() << std::endl;
    return false;
  }
  return true;
}

bool VirtualServer::removeConnection(Connection& connection) {
  t_connections::iterator toRemove = std::find(m_connections.begin(), m_connections.end(), connection);

  if (toRemove == m_connections.end()) {
    return false;
  }
  m_connections.erase(toRemove);
  return true;
}

bool VirtualServer::listen(void) {
  if (this->getSocket().init() == false) return false;
  return true;
}

} /* namespace www */
