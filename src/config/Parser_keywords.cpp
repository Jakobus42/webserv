#include "config/Parser.hpp"
#include "http/http.hpp"

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
	int ConfigFileParser::server(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 1) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for server" << std::endl;
			return 1;
		}
		t_server new_server;
		// set default values
		new_server.port = 80;
		new_server.ip_address = http::LOCALHOST_ADDRESS;
		new_server.max_body_size = 1000000;
		m_configData.servers.push_back(new_server);
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
	int ConfigFileParser::location(std::vector<std::string>& args,
								   const int& lineCount,
								   int layer) {
		if (args.size() != 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for location" << std::endl;
			return 1;
		}
		if (args[1].length() > 1000 || args[1].length() == 0) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Location name length invalid"
					  << std::endl;
			return 1;
		}
		Location new_location;
		// set default values
		new_location.autoindex = false;
		new_location.root = "";
		if (shared::string::splitPath(args[1], new_location.path) == 1) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Invalid location path"
					  << std::endl;
			return 1;
		}
		new_location.return_url = "";
		new_location.index.push_back("index.html");
		if (layer == 1) {
			m_configData.servers.back().locations.push_back(new_location);
			return 0;
		}
		Location* temp = &m_configData.servers.back().locations.back();
		for (int i = 2; i < layer; i++) {
			temp = &temp->locations.back();
		}
		temp->locations.push_back(new_location);
		if (layer > 1) {
			unsigned long paths = 0;
			if (temp->path.size() > new_location.path.size()) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Invalid location path, does not include previous location"
						  << std::endl;
				return 1;
			}
			while (paths < temp->path.size()) {
				if (temp->path[paths] != new_location.path[paths]) {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Invalid location path, does not include previous location"
							  << std::endl;
					return 1;
				}
				paths++;
			}
		}
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
	int ConfigFileParser::listen(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for listen" << std::endl;
			return 1;
		}
		if (is_number(args[1])) {
			int i = ft_stoi(args[1]);
			if (i < 0 || i > 65535) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Invalid number for listen"
						  << std::endl;
				return 1;
			}
			m_configData.servers.back().port = i;
		} else {
			m_configData.servers.back().ip_address = 0;
			for (unsigned long i = 0; i < 4; i++) {
				std::size_t iter;
				if (i == 3) {
					iter = args[1].find(':');
					if (iter != std::string::npos) {
						int j = ft_stoi(args[1].substr(iter + 1));
						if (j < 0 || j > 65535) {
							std::cout << "Configuration file (line "
									  << lineCount << "): "
									  << "Invalid number for listen" << std::endl;
							return 1;
						}
						m_configData.servers.back().port = i;
					}
				} else {
					iter = args[1].find('.');
					if (iter == std::string::npos) {
						std::cout << "Configuration file (line "
								  << lineCount << "): "
								  << "Invalid number for listen" << std::endl;
						return 1;
					}
				}
				std::string temp = args[1].substr(0, iter);
				int j = ft_stoi(temp);
				if (j < 0 || j > 255) {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Invalid number for listen"
							  << std::endl;
					return 1;
				}
				m_configData.servers.back().ip_address += j << (8 * (3 - i));
				args[1] = args[1].substr(iter + 1);
			}
			if (args[1].size() != 0) {
				int i = ft_stoi(args[1]);
				if (i < 0 || i > 65535) {
					std::cout << "Configuration file (line " << lineCount << "): "
							  << "Invalid number for listen"
							  << std::endl;
					return 1;
				}
				m_configData.servers.back().port = i;
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
	int ConfigFileParser::serverName(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;
		if (args.size() < 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for server_name" << std::endl;
			return 1;
		}
		for (unsigned long i = 1; i < args.size(); i++) {
			if (args[i].length() > 1000 || args[i].length() == 0) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Server name length invalid"
						  << std::endl;
				return 1;
			}
			m_configData.servers.back().server_names.push_back(args[i]);
			if (i > 1000) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Too many server names"
						  << std::endl;
				return 1;
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
	int ConfigFileParser::errorPage(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;
		if (args.size() < 3 || args.size() > 1001) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for error_page" << std::endl;
			return 1;
		}
		std::string last = args.back();
		if (last.length() > 1000 || last.length() == 0) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Error page length invalid"
					  << std::endl;
			return 1;
		}
		// TODO: check if last is a valid path?
		args.pop_back();
		for (unsigned long i = 1; i < args.size(); i++) {
			int j = ft_stoi(args[i]);
			if (j < 100 || j > 599) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Invalid number for error_page"
						  << std::endl;
				return 1;
			}
			m_configData.servers.back().errorPages[j] = last;
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
	int ConfigFileParser::clientMaxBodySize(std::vector<std::string>& args,
											int& lineCount,
											int layer) {
		(void)layer;
		if (args.size() != 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for client_max_body_size"
					  << std::endl;
			return 1;
		}
		int i = ft_stoi(args[1]);
		if (i < 0) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number for client_max_body_size" << std::endl;
			return 1;
		}
		m_configData.servers.back().max_body_size = i;
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
	int ConfigFileParser::limitExcept(std::vector<std::string>& args,
									  int& lineCount,
									  int layer) {
		(void)layer;
		if (args.size() < 2 || args.size() > 15) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for limit_except" << std::endl;
			return 1;
		}
		std::vector<std::string> methods;
		methods.push_back("GET");
		methods.push_back("POST");
		methods.push_back("DELETE");
		std::vector<std::string> allowed_methods;
		for (unsigned long i = 1; i < args.size(); i++) {
			for (unsigned long j = 0; j < methods.size(); j++) {
				if (args[i] == methods[j]) {
					allowed_methods.push_back(args[i]);
					methods.erase(methods.begin() + j);
					break;
				}
				if (j == methods.size() - 1) {
					std::cout << "Configuration file (line "
							  << lineCount << "): "
							  << "Invalid method for limit_except" << std::endl;
					return 1;
				}
			}
		}
		if (allowed_methods.size() == 0) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "No methods found for limit_except"
					  << std::endl;
			return 1;
		}
		Location* current = getLocation(layer);
		current->methods = allowed_methods;
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
	int ConfigFileParser::returnKeyword(std::vector<std::string>& args,
										const int& lineCount,
										int layer) {
		if (args.size() != 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for return" << std::endl;
			return 1;
		}
		if (args[1].length() > 1000 || args[1].length() == 0) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Return url length invalid"
					  << std::endl;
			return 1;
		}
		// TODO: check if valid url?
		Location* current = getLocation(layer);
		current->return_url = args[1];
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
	int ConfigFileParser::root(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for root" << std::endl;
			return 1;
		}
		if (args[1].length() > 1000 || args[1].length() == 0) {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Root length invalid" << std::endl;
			return 1;
		}
		// TODO: check if path is valid
		Location* current = getLocation(layer);
		current->root = args[1];
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
	int ConfigFileParser::autoindex(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;
		if (args.size() != 2) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for autoindex" << std::endl;
			return 1;
		}
		Location* current = getLocation(layer);
		if (args[1] == "on") {
			current->autoindex = true;
		} else if (args[1] == "off") {
			current->autoindex = false;
		} else {
			std::cout << "Configuration file (line " << lineCount << "): "
					  << "Invalid argument for autoindex"
					  << std::endl;
			return 1;
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
	int ConfigFileParser::index(std::vector<std::string>& args, int& lineCount, int layer) {
		(void)layer;

		if (args.size() < 2 || args.size() > 1001) {
			std::cout << "Configuration file (line "
					  << lineCount << "): "
					  << "Invalid number of arguments for index" << std::endl;
			return 1;
		}
		Location* current = getLocation(layer);
		current->index.clear();
		for (unsigned long i = 1; i < args.size(); i++) {
			if (args[i].length() > 1000 || args[i].length() == 0) {
				std::cout << "Configuration file (line " << lineCount << "): "
						  << "Index file length invalid"
						  << std::endl;
				return 1;
			}
			current->index.push_back(args[i]);
		}
		return 0;
	}

} // namespace config
