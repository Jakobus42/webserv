#include "http/VirtualServer.hpp"

#include <algorithm>
#include <iostream>

// TODO: make vServer non copyable

namespace http {

	/**
	 * @brief Constructs a new VirtualServer object.
	 */
	VirtualServer::VirtualServer()
		: m_client_max_body_size(ONE_MEGABYTE)
		, m_listen_socket()
		, m_connections() {
		m_listen_socket.create();
		m_listen_socket.bind(8080); // TODO
	}

	/**
	 * @brief Constructs a new VirtualServer object.
	 */
	VirtualServer::VirtualServer(config::t_server& serverConfig)
		: m_client_max_body_size(serverConfig.max_body_size)
		, m_names(serverConfig.server_names)
		, m_locations(serverConfig.locations)
		, m_errorPages(serverConfig.errorPages)
		, m_listen_socket()
		, m_connections() {
		m_listen_socket.create();
		m_listen_socket.bind(serverConfig.port, serverConfig.ip_address);
	}

	/**
	 * @brief Destroys the VirtualServer object.
	 * @todo Should prolly close everything down neatly, this happens only at exit.
	 * @warning Edit: Should NOT close everything, otherwise creating Sockets
	 * @warning and placing them in Reactor's vector triggers the destructor,
	 * @warning closing the socket in the process.
	 */
	VirtualServer::~VirtualServer() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other VirtualServer object to copy.
	 */
	VirtualServer::VirtualServer(const VirtualServer& other)
		: m_client_max_body_size(other.getMaxBodySize())
		, m_names(other.getNames())
		, m_locations(other.getLocations())
		, m_errorPages(other.getErrorPages())
		, m_listen_socket(other.m_listen_socket)
		, m_connections(other.getConnections()) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other VirtualServer object to assign from.
	 * @return A reference to the assigned VirtualServer object.
	 */
	VirtualServer& VirtualServer::operator=(const VirtualServer& rhs) {
		if (this == &rhs)
			return *this;
		m_names = rhs.getNames();
		m_errorPages = rhs.getErrorPages();
		m_locations = rhs.getLocations();
		m_listen_socket = rhs.m_listen_socket;
		m_client_max_body_size = rhs.getMaxBodySize();
		m_connections = rhs.getConnections();
		return *this;
	}

	const std::vector<std::string>& VirtualServer::getNames(void) const {
		return m_names;
	}

	const std::map<int, std::string>& VirtualServer::getErrorPages(void) const {
		return m_errorPages;
	}

	const std::vector<config::t_location>& VirtualServer::getLocations(void) const {
		return m_locations;
	}

	uint64_t VirtualServer::getMaxBodySize(void) const {
		return m_client_max_body_size;
	}

	const Socket& VirtualServer::getSocket(void) const {
		return m_listen_socket;
	}

	Socket& VirtualServer::getSocket(void) {
		return m_listen_socket;
	}

	const t_connections& VirtualServer::getConnections(void) const {
		return m_connections;
	}

	t_connections& VirtualServer::getConnections(void) {
		return m_connections;
	}

	/**
	 * @brief Accepts the next incoming connection in the
	 * VirtualServerSocket's queue
	 *
	 * @return true if the connection could be established
	 * @return false if accept() within Connection->ClientSocket fails
	 */
	bool VirtualServer::addConnection(void) {
		try {
			m_connections.push_back(Connection(m_listen_socket.accept()));
			std::cout << "Added a connection!" << std::endl;
		} catch (std::exception& e) {
			return false;
		}
		return true;
	}

	bool VirtualServer::removeConnection(Connection& connection) {
		t_connections::iterator toRemove =
			std::find(m_connections.begin(), m_connections.end(), connection);

		if (toRemove == m_connections.end()) {
			return false;
		}
		connection.clientSocket.close();
		m_connections.erase(toRemove);
		return true;
	}

	void VirtualServer::listen(void) {
		m_listen_socket.listen();
	}

} // namespace http
