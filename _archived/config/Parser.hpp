#pragma once

#include <stdint.h>

#include "config/Location.hpp"
#include "config/types.hpp"
#include "shared/stringUtils.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

namespace config {


	struct ServerConfig {
			static const std::size_t PATH_MAX_LEN = 8192; // TODO: unused

			// configurations
			int port;
			uint32_t ip_address;
			std::vector<std::string> server_names;
			std::map<int, std::string> errorPages;
			unsigned long max_body_size;
			// locations
			std::vector<Location> locations;
			Location globalRoot; // empty string as globalRoot.root is invalid
			std::string dataDir;

			ServerConfig()
				: port(8080)
				, ip_address(http::LOCALHOST_ADDRESS)
				, server_names()
				, errorPages()
				, max_body_size(1000000)
				, locations()
				, globalRoot()
				, dataDir() {}

			ServerConfig(const ServerConfig& other)
				: port(other.port)
				, ip_address(other.ip_address)
				, server_names(other.server_names)
				, errorPages(other.errorPages)
				, max_body_size(other.max_body_size)
				, locations(other.locations)
				, globalRoot(other.globalRoot)
				, dataDir(other.dataDir) {}

			const ServerConfig& operator=(const ServerConfig& rhs) {
				if (this == &rhs) {
					return *this;
				}
				port = rhs.port;
				ip_address = rhs.ip_address;
				server_names = rhs.server_names;
				errorPages = rhs.errorPages;
				max_body_size = rhs.max_body_size;
				locations = rhs.locations;
				globalRoot = rhs.globalRoot;
				dataDir = rhs.dataDir;
				return *this;
			}

			bool hasGlobalRoot() const { // TODO: should always be true...
				return !globalRoot.root.empty();
			}

			bool hasDataDir() const { // TODO: should always be true...
				return !dataDir.empty();
			}
	};

	/**
	 * @class Parser
	 * @brief This class is responsible for parsing the configuration file
	 */
	class Parser {
		public:
			Parser();
			~Parser();

			int loadConfig(std::string& fileName);

			int getIsLoaded() const;
			const std::vector<ServerConfig>& getServerConfigs() const;
			int getServerSize() const;
			int getServerPort(int index) const;
			uint32_t getServerIp(int index) const;
			std::vector<std::string> getServerNames(int index) const;
			std::map<int, std::string> getErrorPages(int index) const;
			unsigned long getMaxBodySize(int index) const;
			ServerConfig* getServerConfig(int index);

			void printServerConfigs(int detailed);
			void printLocations(const std::vector<config::Location>& locations, int layer, int detailed, std::vector<int> layer_num);
			config::Location* getLocation(int layer);

			static int unexpectedCommandError(int lineCount);
			static int unknownCommandError(int lineCount);
			static int genericError(int lineCount, const std::string& message);

		private:
			Parser(const Parser& other);
			Parser& operator=(const Parser& other);
			std::vector<ServerConfig> m_serverConfigs;
			int m_isLoaded;

			int readFile(std::string& fileName, std::string& file);
			int parseFile(std::string& fileName, int layer, unsigned long& i, int& lineCount);
			int handleServer(std::string& line, unsigned long* i);
			int handlePrompt(std::string& line, int layer, const int& lineCount);
			int saveConfigData(std::vector<std::string>& args, int layer, int qoute_flag, const int& lineCount);
			enum CmdId idCommand(const std::string& command);

			int server(std::vector<std::string>& args, const int& lineCount, int layer);
			int location(std::vector<std::string>& args, const int& lineCount, int layer);

			int listen(std::vector<std::string>& args, const int& lineCount, int layer);
			int serverName(std::vector<std::string>& args, const int& lineCount, int layer);
			int errorPage(std::vector<std::string>& args, const int& lineCount, int layer);
			int clientMaxBodySize(std::vector<std::string>& args, const int& lineCount, int layer);
			int limitExcept(std::vector<std::string>& args, const int& lineCount, int layer);
			int returnKeyword(std::vector<std::string>& args, const int& lineCount, int layer);
			int root(std::vector<std::string>& args, const int& lineCount, int layer);
			int autoindex(const std::vector<std::string>& args, const int& lineCount, int layer);
			int index(std::vector<std::string>& args, const int& lineCount, int layer);
			int globalRoot(std::vector<std::string>& args, const int& lineCount, int layer);
			int dataDir(std::vector<std::string>& args, const int& lineCount, int layer);
			int uploadDir(std::vector<std::string>& args, const int& lineCount, int layer);
	};

} // namespace config
