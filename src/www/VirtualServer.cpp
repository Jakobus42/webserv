#include "../../include/www/VirtualServer.hpp"

namespace www {

/**
 * @brief Constructs a new VirtualServer object.
 */
VirtualServer::VirtualServer() : m_name(""), m_client_max_body_size(ONE_MEGABYTE), m_listen_socket() {}

/**
 * @brief Destroys the VirtualServer object.
 */
VirtualServer::~VirtualServer() {}

// /**
//  * @brief Copy constructor.
//  * @param other The other VirtualServer object to copy.
//  */
// VirtualServer::VirtualServer(const VirtualServer& other)
//     : m_name(other.getName()), m_client_max_body_size(other.getMaxBodySize())
//     {}

// /**
//  * @brief Copy assignment operator.
//  * @param other The other VirtualServer object to assign from.
//  * @return A reference to the assigned VirtualServer object.
//  */
// VirtualServer& VirtualServer::operator=(const VirtualServer& rhs) {
//   if (this == &rhs) return *this;
//   m_name = rhs.getName();
//   m_client_max_body_size = rhs.getMaxBodySize();
//   return *this;
// }

const std::string& VirtualServer::getName(void) const { return m_name; }

uint64_t VirtualServer::getMaxBodySize(void) const { return m_client_max_body_size; }

} /* namespace www */
