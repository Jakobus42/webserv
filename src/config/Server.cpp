#include "config/Server.hpp"

#include "config/GoodParser.hpp"

namespace config {

	/**
	 * @brief Constructs a new Server object.
	 */
	Server::Server()
		: port(8080)
		, ipAddress(http::LOCALHOST_ADDRESS)
		, maxBodySize(ONE_MEGABYTE)
		, dataDirectory("")
		, serverNames()
		, location() {}

	/**
	 * @brief Destroys the Server object.
	 */
	Server::~Server() {}

	/**
	 * @brief Copy constructor.
	 * @param other The other Server object to copy.
	 */
	Server::Server(const Server& other)
		: port(other.port)
		, ipAddress(other.ipAddress)
		, maxBodySize(other.maxBodySize)
		, dataDirectory(other.dataDirectory)
		, serverNames(other.serverNames)
		, location(other.location) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Server object to assign from.
	 * @return A reference to the assigned Server object.
	 */
	const Server& Server::operator=(const Server& rhs) {
		if (this == &rhs) {
			return *this;
		}
		port = rhs.port;
		ipAddress = rhs.ipAddress;
		maxBodySize = rhs.maxBodySize;
		dataDirectory = rhs.dataDirectory;
		serverNames = rhs.serverNames;
		location = rhs.location;
		return *this;
	}

	bool Server::hasGlobalRoot() const { return !location.root.empty(); }

	bool Server::hasDataDir() const { return !dataDirectory.empty(); }

	/**
	 * @brief Validate the server, ensuring it has all mandatory keys
	 * and that nothing else fucky wucky is going on
	 *
	 */
	void Server::validate() const {
		throw parse_exception(1, "fuck");
	}

} /* namespace config */
