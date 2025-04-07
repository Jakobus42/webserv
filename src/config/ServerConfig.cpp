#include "config/ServerConfig.hpp"

#include "config/Parser.hpp"

#include <iostream>

namespace config {

	ServerConfig::ServerConfig()
		: port(8080)
		, ipAddress(LOCALHOST_ADDRESS)
		, maxBodySize(ONE_MEGABYTE)
		, dataDirectory("")
		, dataDirectoryAsTokens()
		, serverNames()
		, location() {}

	ServerConfig::~ServerConfig() {}

	ServerConfig::ServerConfig(const ServerConfig& other)
		: port(other.port)
		, ipAddress(other.ipAddress)
		, maxBodySize(other.maxBodySize)
		, dataDirectory(other.dataDirectory)
		, dataDirectoryAsTokens(other.dataDirectoryAsTokens)
		, serverNames(other.serverNames)
		, location(other.location) {}

	const ServerConfig& ServerConfig::operator=(const ServerConfig& rhs) {
		if (this == &rhs) {
			return *this;
		}
		port = rhs.port;
		ipAddress = rhs.ipAddress;
		maxBodySize = rhs.maxBodySize;
		dataDirectory = rhs.dataDirectory;
		dataDirectoryAsTokens = rhs.dataDirectoryAsTokens;
		serverNames = rhs.serverNames;
		location = rhs.location;
		return *this;
	}

	bool ServerConfig::hasRoot() const { return !location.root.empty(); }

	bool ServerConfig::hasDataDir() const { return !dataDirectory.empty(); }

	/**
	 * @brief Validate the server, ensuring it has all mandatory keys
	 * and that nothing else fucky wucky is going on
	 */
	void ServerConfig::validate() const {
		if (dataDirectory.empty()) {
			throw parse_exception("Server requires a 'data_dir'");
		}
		if (location.root.empty()) {
			throw parse_exception("Server requires a 'root'");
		}
		// TODO: ensure this path exists and can be accessed
	}

	void ServerConfig::print() const {
		std::cout << "--------------------------------" << std::endl;

		std::cout << "Port: " << port << std::endl;
		std::cout << "IPAddress: " << ipAddress << std::endl;
		std::cout << "MaxBodySize: " << maxBodySize << std::endl;
		std::cout << "DataDirectory: " << dataDirectory << std::endl;
		std::cout << "Names: ";
		for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
		this->location.print();

		std::cout << "--------------------------------" << std::endl;
	}

} /* namespace config */
