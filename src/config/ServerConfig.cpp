#include "config/ServerConfig.hpp"

#include "config/Parser.hpp"

#include <iostream>

namespace config {

	ServerConfig::ServerConfig()
		: port(8080)
		, ipAddress(LOCALHOST_ADDRESS)
		, maxBodySize(10 * 1024 * 1024)	 // 10MB
		, maxHeaderValueLength(8 * 1024) // 8KB
		, maxHeaderCount(128)			 // 128 headers
		, maxHeaderValueCount(64)		 // 64 values
		, maxHeaderNameLength(256)		 // 256B
		, connectionTimeout(60)
		, cgiTimeout(60)
		, cgiInterpreters()
		, dataDirectory("")
		, serverNames()
		, location() {}

	ServerConfig::~ServerConfig() {}

	ServerConfig::ServerConfig(const ServerConfig& other)
		: port(other.port)
		, ipAddress(other.ipAddress)
		, socketAddress(other.socketAddress)
		, maxBodySize(other.maxBodySize)
		, maxHeaderValueLength(other.maxHeaderValueLength)
		, maxHeaderCount(other.maxHeaderCount)
		, maxHeaderValueCount(other.maxHeaderValueCount)
		, maxHeaderNameLength(other.maxHeaderNameLength)
		, connectionTimeout(other.connectionTimeout)
		, cgiTimeout(other.cgiTimeout)
		, cgiInterpreters(other.cgiInterpreters)
		, dataDirectory(other.dataDirectory)
		, serverNames(other.serverNames)
		, location(other.location) {}

	const ServerConfig& ServerConfig::operator=(const ServerConfig& rhs) {
		if (this == &rhs) {
			return *this;
		}
		port = rhs.port;
		ipAddress = rhs.ipAddress;
		socketAddress = rhs.socketAddress;
		maxBodySize = rhs.maxBodySize;
		maxHeaderCount = rhs.maxHeaderCount;
		maxHeaderNameLength = rhs.maxHeaderNameLength;
		maxHeaderValueCount = rhs.maxHeaderValueCount;
		maxHeaderValueLength = rhs.maxHeaderValueLength;
		connectionTimeout = rhs.connectionTimeout;
		cgiTimeout = rhs.cgiTimeout;
		cgiInterpreters = rhs.cgiInterpreters;
		dataDirectory = rhs.dataDirectory;
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
		std::cout << "cgiInterpreters:" << std::endl;
		for (std::vector<std::pair<std::string, std::string> >::const_iterator interpreter = cgiInterpreters.begin(); interpreter != cgiInterpreters.end(); ++interpreter) {
			std::cout << interpreter->first << " => " << interpreter->second << std::endl;
		}
		std::cout << "ConnectionTimeout: " << connectionTimeout << std::endl;
		std::cout << "cgiTimeout: " << cgiTimeout << std::endl;
		std::cout << "Names: ";
		for (std::vector<std::string>::const_iterator it = serverNames.begin(); it != serverNames.end(); ++it) {
			std::cout << *it << " ";
		}
		std::cout << std::endl;
		this->location.print();

		std::cout << "--------------------------------" << std::endl;
	}

} /* namespace config */
