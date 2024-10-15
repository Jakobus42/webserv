#include "../../include/testing/MinimalWebserver.hpp"

namespace testing {

/**
 * @brief Constructs a new MinimalWebserver object.
 */
MinimalWebserver::MinimalWebserver(std::string ip, int port)
    : m_ip_address(ip), m_port(port), m_socketAddress_len(sizeof(m_socketAddress)) {
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port = htons(m_port);
  m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address.c_str());
  if (start() != 0) {
    std::cerr << "Cannot start webserver with port" << m_socketAddress.sin_port << std::endl;
  }
}

/**
 * @brief Destroys the MinimalWebserver object.
 */
MinimalWebserver::~MinimalWebserver() { close(); }

/**
 * @brief Starts the MinimalWebserver
 */
int MinimalWebserver::start() {
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0) {
    std::cerr << "Cannot create socket" << std::endl;
    return 1;
  }

  if (bind(m_socket, (struct sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0) {
    std::cerr << "Cannot bind socket" << std::endl;
    return 1;
  }
  return 0;
}

/**
 * @brief Exits the program
 */
void MinimalWebserver::close() {
  // close(m_socket);
  exit(1);
}

void MinimalWebserver::startListen() {
  if (listen(m_socket, 20) < 0) {
    std::cerr << "Socket listen failed" << std::endl;
    exit(1);
  }
  std::cout << "Listening on ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr)
            << " PORT: " << ntohs(m_socketAddress.sin_port) << std::endl;
}

void MinimalWebserver::acceptConnection(int& new_socket) {
  new_socket = accept(m_socket, (sockaddr*)&m_socketAddress, &m_socketAddress_len);
  if (new_socket < 0) {
    std::cerr << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(m_socketAddress.sin_addr)
              << "; PORT: " << ntohs(m_socketAddress.sin_port) << std::endl;
    exit(1);
  }
}

} /* namespace testing */
