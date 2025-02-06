#include "config/Location.hpp"
#include "config/Parser.hpp"
#include "http/Router.hpp"
#include "http/http.hpp"

#include <set>

namespace config {

	/**
	 * @brief Converts a string to a positive integer.
	 * @param str The string to convert.
	 * @return int The integer value of the string. or -1 if the string is not a
	 * number.
	 */
	int ft_stoi(std::string str) {
		long res = 0;
		unsigned long i = 0;
		if (str.size() == 0) {
			return -1;
		}
		for (; i < str.size(); i++) {
			if (!std::isdigit(str[i]))
				return -1;
			res = res * 10 + str[i] - '0';
			if (res > 2147483647)
				return -1;
		}
		return res;
	}

	/**
	 * @brief Checks if a string is a number.
	 * @param str The string to check.
	 * @return true if the string is a number, false otherwise.
	 */
	bool is_number(const std::string& str) {
		std::string::const_iterator it = str.begin();
		while (it != str.end() && std::isdigit(*it))
			++it;
		if (it == str.end())
			return true;
		return false;
	}

	/**
	 * @brief Creates a new server struct, initializes it and adds it to the config
	 * data.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::server(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 1) {
			return genericError(lineCount, "Invalid number of arguments for server");
		}
		ServerConfig new_server;
		// set default values
		new_server.port = 80;
		new_server.ip_address = http::LOCALHOST_ADDRESS;
		new_server.max_body_size = 1000000;
		m_serverConfigs.push_back(new_server);
		// TODO: Load default Error pages if none are provided
		return 0;
	}

	/**
	 * @brief Creates a new location struct, initializes it and adds it to the
	 * config data.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::location(std::vector<std::string>& args, const int& lineCount, int layer) {
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for location");
		}
		if (args[1].length() > 1000 || args[1].length() == 0) {
			return genericError(lineCount, "Location name length invalid");
		}
		Location new_location;
		// set default values
		new_location.autoindex = false;
		new_location.root.clear();
		if (shared::string::splitPath(args[1], new_location.path) == 1) {
			return genericError(lineCount, "Invalid location path");
		}
		new_location.redirectUri = "";
		new_location.indexFile.push_back("index.html");
		if (layer == 1) {
			m_serverConfigs.back().locations.push_back(new_location);
			return 0;
		}
		Location* temp = &m_serverConfigs.back().locations.back();
		for (int i = 2; i < layer; i++) {
			temp = &temp->locations.back();
		}
		temp->locations.push_back(new_location);
		// if (layer > 1) {
		// 	unsigned long paths = 0;
		// 	if (temp->path.size() >= new_location.path.size()) {
		// 		std::cout << "Configuration file (line " << lineCount << "): "
		// 				  << "Invalid location path, does not include previous location"
		// 				  << std::endl;
		// 		return 1;
		// 	}
		// 	while (paths < temp->path.size()) {
		// 		if (temp->path[paths] != new_location.path[paths]) {
		// 			std::cout << "Configuration file (line " << lineCount << "): "
		// 					  << "Invalid location path, does not include previous location"
		// 					  << std::endl;
		// 			return 1;
		// 		}
		// 		paths++;
		// 	}
		// }
		return 0;
	}

	/**
	 * @brief Sets the port and ip address of the server.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::listen(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for listen");
		}
		if (is_number(args[1])) {
			int i = ft_stoi(args[1]);
			if (i < 0 || i > 65535) {
				return genericError(lineCount, "Invalid number for listen");
			}
			m_serverConfigs.back().port = i;
		} else {
			m_serverConfigs.back().ip_address = 0;
			for (unsigned long i = 0; i < 4; i++) {
				std::size_t iter;
				if (i == 3) {
					iter = args[1].find(':');
					if (iter != std::string::npos) {
						int j = ft_stoi(args[1].substr(iter + 1));
						if (j < 0 || j > 65535) {
							return genericError(lineCount, "Invalid number for listen");
						}
						m_serverConfigs.back().port = i;
					}
				} else {
					iter = args[1].find('.');
					if (iter == std::string::npos) {
						return genericError(lineCount, "Invalid number for listen");
					}
				}
				std::string temp = args[1].substr(0, iter);
				int j = ft_stoi(temp);
				if (j < 0 || j > 255) {
					return genericError(lineCount, "Invalid number for listen");
				}
				m_serverConfigs.back().ip_address += j << (8 * (3 - i));
				args[1] = args[1].substr(iter + 1);
			}
			if (args[1].size() != 0) {
				int i = ft_stoi(args[1]);
				if (i < 0 || i > 65535) {
					return genericError(lineCount, "Invalid number for listen");
				}
				m_serverConfigs.back().port = i;
			}
		}
		return 0;
	}

	/**
	 * @brief Sets the server name(s) of the server.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::serverName(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;
		if (args.size() < 2) {
			return genericError(lineCount, "Invalid number of arguments for server_name");
		}
		for (unsigned long i = 1; i < args.size(); i++) {
			if (args[i].length() > 1000 || args[i].length() == 0) {
				return genericError(lineCount, "Server name length invalid");
			}
			m_serverConfigs.back().server_names.push_back(args[i]);
			if (i > 1000) {
				return genericError(lineCount, "Too many server names");
			}
		}
		return 0;
	}

	/**
	 * @brief sets the error pages for the server.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::errorPage(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;
		if (args.size() < 3 || args.size() > 1001) {
			return genericError(lineCount, "Invalid number of arguments for error_page");
		}
		std::string last = args.back();
		if (last.length() > 1000 || last.length() == 0) {
			return genericError(lineCount, "Error page length invalid");
		}
		// TODO: check if last is a valid path?
		args.pop_back();
		for (unsigned long i = 1; i < args.size(); i++) {
			int j = ft_stoi(args[i]);
			if (j < 100 || j > 599) {
				return genericError(lineCount, "Invalid number for error_page");
			}
			m_serverConfigs.back().errorPages[j] = last;
		}
		return 0;
	}

	/**
	 * @brief sets the max body size for the server.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::clientMaxBodySize(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for client_max_body_size");
		}
		int i = ft_stoi(args[1]);
		if (i < 0) {
			return genericError(lineCount, "Invalid number for client_max_body_size");
		}
		m_serverConfigs.back().max_body_size = i;
		return 0;
	}

	/**
	 * @brief sets the methods allowed for the location.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::limitExcept(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;
		if (args.size() < 2 || args.size() > 15) {
			return genericError(lineCount, "Invalid number of arguments for limit_except");
		}
		std::map<std::string, http::Method> methods;
		std::set<http::Method> foundMethods;
		methods.insert(std::make_pair("GET", http::GET));
		methods.insert(std::make_pair("POST", http::POST));
		methods.insert(std::make_pair("DELETE", http::DELETE));

		for (unsigned long i = 1; i < args.size(); i++) {
			std::map<std::string, http::Method>::iterator it = methods.find(args[i]);
			if (it != methods.end() && foundMethods.find(it->second) == foundMethods.end()) {
				foundMethods.insert(it->second);
				std::cout << "Found method: " << it->first << ": " << it->second << std::endl;
				std::cout << *foundMethods.find(it->second);
				methods.erase(methods.find(args[i]));
			} else {
				return genericError(lineCount, "Invalid method for limit_except");
			}
		}
		if (foundMethods.empty()) {
			return genericError(lineCount, "No methods found for limit_except");
		}
		Location* current = getLocation(layer);
		current->allowedMethods = foundMethods;
		return 0;
	}

	/**
	 * @brief redirects the user to a different url with a specific return code.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::returnKeyword(std::vector<std::string>& args, const int& lineCount, int layer) {
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for return");
		}
		if (args[1].length() > 1000 || args[1].length() == 0) {
			return genericError(lineCount, "Return url length invalid");
		}
		if (args[1][0] != '/') {
			return genericError(lineCount, "Return url doesn't start with '/'");
		}
		// TODO: check if valid url?
		Location* current = getLocation(layer);
		current->redirectUri = args[1];
		current->redirectUriTokens = http::Router::splitPath(current->redirectUri);
		return 0;
	}

	/**
	 * @brief sets the root directory for the location.
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::root(std::vector<std::string>& args, const int& lineCount, int layer) {
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for root");
		}
		if (args[1].length() > 1000 || args[1].length() == 0) {
			return genericError(lineCount, "Root length invalid");
		}
		// TODO: check if path is valid
		Location* current = getLocation(layer);
		current->root = http::Router::splitPath(args[1]);
		return 0;
	}

	/**
	 * @brief turns the autoindex on or off for the location. (off by default)
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::autoindex(const std::vector<std::string>& args, const int& lineCount, int layer) {
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for autoindex");
		}
		Location* current = getLocation(layer);
		if (args[1] == "on") {
			current->autoindex = true;
		} else if (args[1] == "off") {
			current->autoindex = false;
		} else {
			return genericError(lineCount, "Invalid argument for autoindex");
		}
		return 0;
	}

	/**
	 * @brief sets the index files for the location. (index.html by default)
	 * @param args all prompts in the current line of the configuration file.
	 * @param lineCount the current line number, so we are able to print error
	 * messages.
	 * @param layer the current layer of the configuration file.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::index(std::vector<std::string>& args, const int& lineCount, int layer) {
		(void)layer;

		if (args.size() < 2 || args.size() > 1001) {
			return genericError(lineCount, "Invalid number of arguments for index");
		}
		Location* current = NULL;
		if (layer == 0) {
			ServerConfig& server = m_serverConfigs.back();
			current = &server.globalRoot; // current is globalRootLocation
		} else {
			current = getLocation(layer);
		}
		current->indexFile.clear();
		for (unsigned long i = 1; i < args.size(); i++) {
			if (args[i].length() > 1000 || args[i].length() == 0) {
				return genericError(lineCount, "Index file length invalid");
			}
			current->indexFile.push_back(args[i]);
		}
		return 0;
	}

	int ConfigFileParser::globalRoot(std::vector<std::string>& args, const int& lineCount, int layer) {
		if (layer != 1) {
			return genericError(lineCount, "'global_root' is only allowed in server blocks");
		}
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for global_root");
		}
		if (args[1].empty() || args[1].length() > 1000) {
			return genericError(lineCount, "global_root path length invalid");
		}
		if (args[1][0] != '/') {
			return genericError(lineCount, "global_root path invalid, doesn't start with '/'");
		}
		if (args[1].find("//") != std::string::npos) {
			return genericError(lineCount, "global_root path invalid, duplicate '/'");
		}
		if (args[1][args[1].size() - 1] == '/') {
			return genericError(lineCount, "global_root path invalid, ends with '/'");
		}

		ServerConfig& server = m_serverConfigs.back();
		if (server.hasGlobalRoot()) {
			return genericError(lineCount, "Duplicate global_root directive");
		}

		// TODO: validate if we can access global root path?
		// TODO: probably should not start the server unless it is

		server.globalRoot.root = http::Router::splitPath(args[1]); // TODO: Location.root or Location.path?
		server.globalRoot.allowedMethods.clear();
		if (server.hasDataDir()) {
			std::vector<std::string> dataDir = http::Router::splitPath(server.dataDir);
			server.globalRoot.root.insert(server.globalRoot.root.end(), dataDir.begin(), dataDir.end());
			// server.globalRoot.path.push_back(server.globalRoot.root + server.dataDir);
			// TODO: concatenate and then split up globalRoot.root and server.dataDir;
		}
		// server.globalRoot.path = args[1] + "/www"; // TODO: set server.globalRoot.path to <globalRoot.root>/www (or another subfolder in globalRoot)
		return 0;
	}

	int ConfigFileParser::dataDir(std::vector<std::string>& args, const int& lineCount, int layer) {
		if (layer != 1) {
			return genericError(lineCount, "'data_dir' is only allowed in server blocks");
		}
		if (args.size() != 2) {
			return genericError(lineCount, "Invalid number of arguments for data_dir");
		}
		if (args[1].empty() || args[1].length() > 1000) {
			return genericError(lineCount, "data_dir path length invalid");
		}
		if (args[1][0] != '/') {
			return genericError(lineCount, "data_dir path invalid, doesn't start with '/'");
		}
		if (args[1].find("//") != std::string::npos) {
			return genericError(lineCount, "data_dir path invalid, duplicate '/'");
		}
		if (args[1][args[1].size() - 1] == '/') {
			return genericError(lineCount, "data_dir path invalid, ends with '/'");
		}

		ServerConfig& server = m_serverConfigs.back();
		if (server.hasDataDir()) {
			return genericError(lineCount, "Duplicate data_dir directive");
		}

		// TODO: validate if we can access global root path?
		// TODO: probably should not start the server unless it is

		server.dataDir = args[1];
		if (server.hasGlobalRoot()) {
			std::vector<std::string> dataDir = http::Router::splitPath(server.dataDir);
			server.globalRoot.root.insert(server.globalRoot.root.end(), dataDir.begin(), dataDir.end());
			// server.globalRoot.path.push_back(server.globalRoot.root + server.dataDir);
			// TODO: concatenate and then split up globalRoot.root and server.dataDir;
		}
		return 0;
	}

} // namespace config
