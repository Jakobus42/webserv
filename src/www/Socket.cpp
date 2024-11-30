#include "../../include/www/Socket.hpp"

#include <iostream>
#include <vector>

namespace www {

const uint32_t LOCALHOST_ADDRESS = 0x7F000001;  // 127.0.0.1

/**
 * @brief Constructs a new Socket object for port 80.
 */
Socket::Socket() : m_fd(-1), m_port(80) {
  // port better not be negative
  struct sockaddr_in in_address;
  int opt = 1;

  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) throw std::exception();
  if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    ::close(m_fd);
    throw std::exception();
  }

  in_address.sin_family = AF_INET;
  in_address.sin_port = htons(m_port);
  in_address.sin_addr.s_addr = htonl(LOCALHOST_ADDRESS);  // 127.0.0.1
  if (::bind(m_fd, (struct sockaddr*)&in_address, sizeof(in_address)) < 0) {
    ::close(m_fd);
    throw std::exception();
  }
  if (listen(m_fd, 10) < 0) {
    ::close(m_fd);
    throw std::exception();
  }
  std::cout << "Successfully created Socket!" << std::endl;
}

/**
 * @brief Constructs a new Socket object for a specified port.
 */
Socket::Socket(int port) : m_fd(-1), m_port(port) {
  // port better not be negative
  struct sockaddr_in in_address;
  int opt = 1;

  m_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_fd == -1) throw std::exception();
  if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    ::close(m_fd);
    throw std::exception();
  }

  in_address.sin_family = AF_INET;
  in_address.sin_port = htons(m_port);
  in_address.sin_addr.s_addr = htonl(LOCALHOST_ADDRESS);  // 127.0.0.1
  if (::bind(m_fd, (struct sockaddr*)&in_address, sizeof(in_address)) < 0) {
    ::close(m_fd);
    throw std::exception();
  }
  if (listen(m_fd, 10) < 0) {
    ::close(m_fd);
    throw std::exception();
  }
  std::cout << "Successfully created Socket!" << std::endl;
}

/**
 * @brief Destroys the Socket object.
 */
Socket::~Socket() {
  // close(m_fd);
}

/**
 * @brief Copy constructor.
 * @param other The other Socket object to copy.
 */
Socket::Socket(const Socket& other) { operator=(other); }

/**
 * @brief Copy assignment operator.
 * @param other The other Socket object to assign from.
 * @throws
 * @return A reference to the assigned Socket object.
 */
Socket& Socket::operator=(const Socket& rhs) {
  if (this == &rhs) return *this;
  m_fd = rhs.getFd();
  m_port = rhs.getPort();
  return *this;
}

int Socket::getFd() const { return m_fd; }

int Socket::getPort() const { return m_port; }

void Socket::close(void) { ::close(m_fd); }

// bool Socket::init() {
// 	int opt = 1;
// 	m_fd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (m_fd == -1)
// 		return false;
// 	if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
// 		close(m_fd);
// 		m_fd = -1;
// 		return false;
// 	}
// 	return true;
// }

// bool Socket::bind() {
// 	struct sockaddr_in in_address;
// 	in_address.sin_family = AF_INET;

// 	if (::bind(m_fd, (struct sockaddr *)&in_address, sizeof(in_address)) < 0) {
// 		close(m_fd);
// 		m_fd = -1;
// 		return false;
// 	}
// 	if (listen(m_fd, 10) < 0) {
// 		close(m_fd);
// 		m_fd = -1;
// 		return false;
// 	}
// 	return true;
// }

struct SocketPile {
 private:
  std::vector<Socket> m_sockets;

 public:
  Socket& at(size_t);
  void addSocket(int port);
  Socket& operator[](size_t index);
  // throwing Socket constructor then catch in container class
};

Socket& SocketPile::at(size_t index) { return m_sockets[index]; }

Socket& SocketPile::operator[](size_t index) { return m_sockets[index]; }

void SocketPile::addSocket(int port = 80) {
  try {
    m_sockets.push_back(Socket(port));
  } catch (std::exception& e) {
    std::cout << "Oh no, socketpile failed to make a socket!" << std::endl;
  }
}

} /* namespace www */
