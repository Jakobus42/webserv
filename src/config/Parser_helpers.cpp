#include "config/Parser.hpp"
#include "http/http.hpp"

namespace config {

	/**
	 * @brief parses a line in the configuration file.
	 * @param line the line to parse.
	 * @param layer the current layer of the configuration file.
	 * @param lineCount the current line number.
	 * @return int 0 if successful, 1 if not.
	 */
	int ConfigFileParser::handlePrompt(std::string& line, int layer, int& lineCount) {
		// return 0 if successful and semicolon is found, 1 if semicolon is not
		// found, 2 if error cut off trailing and starting spaces
		int qouteFlag = 1;
		while (line[line.size() - 1] == ' ') {
			line.erase(line.size() - 1);
		}
		while (line[0] == ' ') {
			line.erase(0, 1);
		}
		if (line[line.size() - 1] == ';') {
			qouteFlag = 0;
			line.erase(line.size() - 1);
		}
		if (line.find(';') != std::string::npos) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Missplaced semicolon (;) found"
					  << std::endl;
			return 2;
		}
		std::vector<std::string> args;
		std::stringstream stream(line);
		std::string key;
		while (getline(stream, key, ' ')) {
			args.push_back(key);
		}
		if (saveConfigData(args, layer, qouteFlag, lineCount) == 1) {
			return 2;
		}
		return qouteFlag;
	}

	/**
	 * @brief finds the current location in the configuration file.
	 *
	 * @param layer the current layer of the configuration file.
	 * @return struct location* the current location.
	 */
	Location* ConfigFileParser::getLocation(int layer) {
		config::Location* temp = &m_serverConfigs.back().locations.back();
		for (int i = 2; i < layer; i++) {
			temp = &temp->locations.back();
		}
		return temp;
	}

	/**
	 * @brief prints the configuration data.
	 * @param detailed toggles between detailed and simple output. (0 for simple, 1
	 * for detailed)
	 */
	void ConfigFileParser::printServerConfigs(int detailed) {
		if (m_isLoaded == 0) {
			std::cout << "No configuration file loaded" << std::endl;
			return;
		}
		for (unsigned long i = 0; i < m_serverConfigs.size(); i++) {
			std::cout << "--------------------------" << std::endl;
			std::cout << "Server: " << i + 1 << std::endl;
			if (detailed) {
				std::cout << "Port: " << m_serverConfigs[i].port << std::endl;
				std::cout << "IP address: ";
				std::cout << m_serverConfigs[i].ip_address << std::endl;
				std::cout << std::endl;
				std::cout << "Server names: ";
				for (unsigned long j = 0;
					 j < m_serverConfigs[i].server_names.size();
					 j++) {
					std::cout << m_serverConfigs[i].server_names[j] << " ";
				}
				std::cout << std::endl;
				std::cout << "Error pages: ";
				for (std::map<int, std::string>::iterator it =
						 m_serverConfigs[i].errorPages.begin();
					 it != m_serverConfigs[i].errorPages.end();
					 ++it) {
					std::cout << it->first << " " << it->second << " ";
				}
				std::cout << std::endl;
				std::cout << "Max body size: "
						  << m_serverConfigs[i].max_body_size << std::endl;
				std::cout << "Locations: " << std::endl;
			}
			std::vector<int> layer_num;
			printLocations(m_serverConfigs[i].locations, 1, detailed, layer_num);
		}
	}

	/**
	 * @brief prints the locations of the server.
	 * @param locations the locations to print.
	 * @param layer the current layer of the configuration file.
	 * @param detailed toggles between detailed and simple output. (0 for simple, 1
	 * for detailed)
	 * @param layer_num the current layer number.
	 */
	void ConfigFileParser::printLocations(
		const std::vector<config::Location>& locations, int layer, int detailed, std::vector<int> layer_num) {
		std::string c = "";
		for (int i = 0; i < layer; i++) {
			c += "   ";
			c += "|";
		}
		layer_num.push_back(1);
		for (unsigned long j = 0; j < locations.size(); j++) {
			std::cout << "--------------------------" << std::endl;
			std::cout << c << "Location: ";
			for (unsigned long k = 0; k < layer_num.size(); k++) {
				std::cout << layer_num[k];
				if (k != layer_num.size() - 1)
					std::cout << ".";
			}
			for (std::vector<std::string>::const_iterator it = locations[j].path.begin();
				 it != locations[j].path.end();
				 ++it) {
				std::cout << c << "/" << *it << std::endl;
			}
			std::cout << std::endl;
			if (detailed) {
				if (locations[j].root != "")
					std::cout << c << "Root: " << locations[j].root << std::endl;
				if (locations[j].autoindex)
					std::cout << c << "Autoindex: on" << std::endl;
				else
					std::cout << c << "Autoindex: off" << std::endl;
				if (locations[j].methods.size() > 0) {
					std::cout << c << "Methods: ";
					for (unsigned long k = 0; k < locations[j].methods.size();
						 k++) {
						std::cout << locations[j].methods[k] << " ";
					}
					std::cout << std::endl;
				}
				if (locations[j].index.size() > 0) {
					std::cout << c << "Index: ";
					for (unsigned long k = 0; k < locations[j].index.size(); k++) {
						std::cout << locations[j].index[k] << " ";
					}
					std::cout << std::endl;
				}
			}
			// go one step deeper
			if (locations[j].locations.size() > 0) {
				// layer_num.push_back(1);
				printLocations(locations[j].locations, layer + 1, detailed, layer_num);
			}
			layer_num.back()++;
		}
	}

	int ConfigFileParser::testFunction(const std::string& key,
									   std::vector<std::string>& args,
									   int& lineCount) {
		int i;
		if (key == "server")
			i = server(args, lineCount, 0);
		else if (key == "listen")
			i = listen(args, lineCount, 1);
		else if (key == "server_name")
			i = serverName(args, lineCount, 1);
		else if (key == "error_page")
			i = errorPage(args, lineCount, 1);
		else if (key == "client_max_body_size")
			i = clientMaxBodySize(args, lineCount, 1);
		else if (key == "location")
			i = location(args, lineCount, 1);
		else if (key == "limit_except")
			i = limitExcept(args, lineCount, 2);
		else if (key == "return")
			i = returnKeyword(args, lineCount, 2);
		else if (key == "root")
			i = root(args, lineCount, 2);
		else if (key == "autoindex")
			i = autoindex(args, lineCount, 2);
		else if (key == "index")
			i = index(args, lineCount, 2);
		else
			return 2;
		return i;
	}

	/**
	 * @brief Identifies the command in the configuration file. and returns the id
	 * of the command.
	 * @param key The key to identify.
	 * @return The id of the command. (1 if listen, 2 if server_name, 3 if
	 * error_page, 4 if client_max_body_size, 5 if location, 6 if limit_exept, 7 if
	 * return, 8 if root, 9 if autoindex, 10 if index, 404 if not found.)
	 */
	enum CmdId ConfigFileParser::idCommand(const std::string& key) {
		if (key == "server")
			return SERVER_ID;
		else if (key == "listen")
			return LISTEN_ID;
		else if (key == "server_name")
			return SERVER_NAME_ID;
		else if (key == "error_page")
			return ERROR_PAGE_ID;
		else if (key == "client_max_body_size")
			return CLIENT_MAX_BODY_SIZE_ID;
		else if (key == "location")
			return LOCATION_ID;
		else if (key == "limit_except")
			return LIMIT_EXCEPT_ID;
		else if (key == "return")
			return RETURN_ID;
		else if (key == "root")
			return ROOT_ID;
		else if (key == "autoindex")
			return AUTOINDEX_ID;
		else if (key == "index")
			return INDEX_ID;
		else
			return UNKNOWN_ID;
	}

	/**
	 * @brief Saves the configuration data.
	 * @param args The arguments to save.
	 * @param layer The layer of the configuration file.
	 * @param qoute_flag The flag to check if quotes are found. 0 if found, 1 if not
	 * found.
	 * @param lineCount The current line number.
	 * @return 0 if successful, 1 if error.
	 */
	int ConfigFileParser::saveConfigData(std::vector<std::string>& args, int layer, int qoute_flag, int& lineCount) {
		enum CmdId command_id = idCommand(args[0]);
		if (command_id == UNKNOWN_ID) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Unknown command found" << std::endl;
			return 1;
		}
		if (layer == 0) // global
		{
			if (qoute_flag == 0) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Unexpected command found"
						  << std::endl;
				return 1;
			} else {
				if (command_id == SERVER_ID) {
					if (server(args, lineCount, layer) == 1)
						return 1;
				} else {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Unexpected command found"
							  << std::endl;
					return 1;
				}
			}
		} else if (layer == 1) // server
		{
			if (qoute_flag == 0) {
				if (command_id == LISTEN_ID) {
					if (listen(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == SERVER_NAME_ID) {
					if (serverName(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == ERROR_PAGE_ID) {
					if (errorPage(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == CLIENT_MAX_BODY_SIZE_ID) {
					if (clientMaxBodySize(args, lineCount, layer) == 1)
						return 1;
				} else {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Unexpected command found"
							  << std::endl;
					return 1;
				}
			} else {
				if (command_id == LOCATION_ID) {
					if (location(args, lineCount, layer) == 1)
						return 1;
				} else {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Unexpected command found"
							  << std::endl;
					return 1;
				}
			}
		} else if (layer > 1) // location
		{
			if (qoute_flag == 0) {
				if (command_id == LIMIT_EXCEPT_ID) {
					if (limitExcept(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == RETURN_ID) {
					if (returnKeyword(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == ROOT_ID) {
					if (root(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == AUTOINDEX_ID) {
					if (autoindex(args, lineCount, layer) == 1)
						return 1;
				} else if (command_id == INDEX_ID) {
					if (index(args, lineCount, layer) == 1)
						return 1;
				} else {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Unexpected command found"
							  << std::endl;
					return 1;
				}
			} else {
				if (command_id == LOCATION_ID) {
					if (location(args, lineCount, layer) == 1)
						return 1;
				} else {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Unexpected command found"
							  << std::endl;
					return 1;
				}
			}
		}
		return 0;
	}

} // namespace config
