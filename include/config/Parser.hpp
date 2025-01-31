#pragma once

#include <stdint.h>

#include "config/Parser.hpp"
#include "http/http.hpp"
#include "shared/stringUtils.hpp"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#define PATH_MAX 8192

namespace config {

	struct Location {
			std::vector<std::string> path;	  // location name
			std::string redirectUrl;		  // return
			std::vector<std::string> methods; // configurations
			bool autoindex;
			std::string root;
			std::vector<std::string> index;
			std::vector<Location> locations; // locations

			// globalRoot should always have path <global_root>/www/
			// what about defaults for accepted methods?
			// should globalRoot even be a Location?
			// or just a string that's used as a base for all routes?

			// default constructor
			Location()
				: path()
				, redirectUrl()
				, methods()
				, autoindex(false)
				, root()
				, index()
				, locations() {
			}

			// copy constructor
			Location(const Location& other)
				: path(other.path)
				, redirectUrl(other.redirectUrl)
				, methods(other.methods)
				, autoindex(other.autoindex)
				, root(other.root)
				, index(other.index)
				, locations(other.locations) {
			}

			// assignment operator
			Location& operator=(const Location& other) {
				if (this != &other) {
					path = other.path;
					redirectUrl = other.redirectUrl;
					methods = other.methods;
					autoindex = other.autoindex;
					root = other.root;
					index = other.index;
					locations = other.locations;
				}
				return *this;
			}

			bool acceptsFileUpload() const {
				return std::find(methods.begin(), methods.end(), "POST") != methods.end(); // TODO: check whether find actually returns end on fail xd
			}

			bool hasRedirect() const {
				return !redirectUrl.empty();
			}
	};

	enum CmdId {
		SERVER_ID = 0,
		LISTEN_ID = 1,
		SERVER_NAME_ID = 2,
		ERROR_PAGE_ID = 3,
		CLIENT_MAX_BODY_SIZE_ID = 4,
		LOCATION_ID = 5,
		LIMIT_EXCEPT_ID = 6,
		RETURN_ID = 7,
		ROOT_ID = 8,
		AUTOINDEX_ID = 9,
		INDEX_ID = 10,
		GLOBAL_ROOT_ID = 11,
		UNKNOWN_ID = 404
	};

	struct ServerConfig {
			// configurations
			int port;
			uint32_t ip_address;
			std::vector<std::string> server_names;
			std::map<int, std::string> errorPages;
			unsigned long max_body_size;
			// locations
			std::vector<Location> locations;
			std::string globalRoot; // empty string is invalid

			ServerConfig()
				: port(8080)
				, ip_address(http::LOCALHOST_ADDRESS)
				, server_names()
				, errorPages()
				, max_body_size(1000000)
				, locations()
				, globalRoot("") {}

			ServerConfig(const ServerConfig& other)
				: port(other.port)
				, ip_address(other.ip_address)
				, server_names(other.server_names)
				, errorPages(other.errorPages)
				, max_body_size(other.max_body_size)
				, locations(other.locations)
				, globalRoot(other.globalRoot) {}

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
				return *this;
			}

			bool hasGlobalRoot() const {
				return !globalRoot.empty();
			}
	};

	/**
	 * @class ConfigFileParser
	 * @brief This class is responsible for parsing the configuration file
	 */
	class ConfigFileParser {
		public:
			ConfigFileParser();
			~ConfigFileParser();

			int loadConfigFile(std::string& configFileName);

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
			int testFunction(const std::string& key, std::vector<std::string>& args, int& lineCount);
			void printLocations(const std::vector<config::Location>& locations,
								int layer,
								int detailed,
								std::vector<int> layer_num);
			config::Location* getLocation(int layer);

		private:
			ConfigFileParser(const ConfigFileParser& other);
			ConfigFileParser& operator=(const ConfigFileParser& other);
			std::vector<ServerConfig> m_serverConfigs;
			int m_isLoaded;

			int readConfigFile(std::string& configFileName, std::string& file);
			int parseConfigFile(std::string& configFileName, int layer, unsigned long& i, int& lineCount);
			int handleServer(std::string& line, unsigned long* i);
			int handlePrompt(std::string& line, int layer, int& lineCount);
			int saveConfigData(std::vector<std::string>& args, int layer, int qoute_flag, int& lineCount);
			enum CmdId idCommand(const std::string& command);

			int server(std::vector<std::string>& args, int& lineCount, int layer);
			int location(std::vector<std::string>& args, const int& lineCount, int layer);

			int listen(std::vector<std::string>& args, int& lineCount, int layer);
			int serverName(std::vector<std::string>& args, int& lineCount, int layer);
			int errorPage(std::vector<std::string>& args, int& lineCount, int layer);
			int clientMaxBodySize(std::vector<std::string>& args, int& lineCount, int layer);
			int limitExcept(std::vector<std::string>& args, int& lineCount, int layer);
			int returnKeyword(std::vector<std::string>& args, const int& lineCount, int layer);
			int root(std::vector<std::string>& args, int& lineCount, int layer);
			int autoindex(std::vector<std::string>& args, int& lineCount, int layer);
			int index(std::vector<std::string>& args, int& lineCount, int layer);
			int globalRoot(std::vector<std::string>& args, int& lineCount, int layer);
	};

} // namespace config
