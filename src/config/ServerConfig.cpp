#include "config/ServerConfig.hpp"

#include "config/ParseException.hpp"

#include <iostream>

namespace config {

	ServerConfig::ServerConfig(const HttpConfig& globalConfig)
		: port(8080)
		, ipAddress(LOCALHOST_ADDRESS)
		, serverNames()
		, location()
		, global(globalConfig) {}

	ServerConfig::~ServerConfig() {}

	ServerConfig::ServerConfig(const ServerConfig& other)
		: port(other.port)
		, ipAddress(other.ipAddress)
		, socketAddress(other.socketAddress)
		, serverNames(other.serverNames)
		, location(other.location)
		, global(other.global) {}

	const ServerConfig& ServerConfig::operator=(const ServerConfig& rhs) {
		if (this == &rhs) {
			return *this;
		}
		port = rhs.port;
		ipAddress = rhs.ipAddress;
		socketAddress = rhs.socketAddress;
		serverNames = rhs.serverNames;
		location = rhs.location;
		return *this;
	}

	/**
	 * @brief Validate the server, ensuring it has all mandatory keys
	 * and that nothing else fucky wucky is going on
	 */
	void ServerConfig::validate() const {
		if (location.root.empty()) {
			throw ParseException("Server requires a 'root'");
		}
		// TODO: ensure this path exists and can be accessed
	}

	void ServerConfig::print() const {
		std::cout << "--------------------------------" << std::endl;

		std::cout << "Port: " << port << std::endl;
		std::cout << "IPAddress: " << ipAddress << std::endl;
		std::cout << "cgiInterpreters:" << std::endl;
		std::cout << "Names: ";
		for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
		this->location.print();

		std::cout << "--------------------------------" << std::endl;
	}

} /* namespace config */
