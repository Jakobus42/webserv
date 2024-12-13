#include "config/Parser.hpp"

namespace config {

	/**
	 * @brief Constructs a new ConfigFileParser object.
	 */
	ConfigFileParser::ConfigFileParser() {
		m_isLoaded = 0;
	}

	/**
	 * @brief Destroys the ConfigFileParser object.
	 */
	ConfigFileParser::~ConfigFileParser() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other ConfigFileParser object to copy.
	 */
	ConfigFileParser::ConfigFileParser(const ConfigFileParser&) {
		m_isLoaded = 0;
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other ConfigFileParser object to assign from.
	 * @return A reference to the assigned ConfigFileParser object.
	 */
	ConfigFileParser& ConfigFileParser::operator=(const ConfigFileParser&) {
		m_isLoaded = 0;
		return *this;
	}

	/**
	 * @brief Reads the configuration file.
	 * @param configFileName The name of the configuration file.
	 * @param line the location where the configuration file is stored by the read.
	 */
	int ConfigFileParser::readConfigFile(std::string& configFileName,
										 std::string& line) {
		if (configFileName.empty()) {
			LOG("Configuration file name is empty." << std::endl, 1)
			return 1;
		}
		std::ifstream infile;
		infile.open(configFileName.c_str());
		char c;
		if (!infile.is_open()) {
			LOG("Could not open configuration file." << std::endl, 1)
			return 1;
		}
		while (!infile.eof() && infile >> std::noskipws >> c) {
			if (c == '#') {
				while (c != '\n') {
					infile >> std::noskipws >> c;
				}
				line += '\n';
			} else {
				line += c;
			}
		}
		infile.close();
		if (infile.is_open()) {
			LOG("Could not close Configuration file" << std::endl, 1)
			return 1;
		}
		if (line.empty()) {
			LOG("Configuration file is empty." << std::endl, 1)
			return 1;
		}
		for (unsigned long b = 0; b < line.size(); b++) {
			if (line[b] == '\t' || line[b] == '\r' || line[b] == '\v' ||
				line[b] == '\f')
				line[b] = ' ';
		}
		line += '\n';
		return 0;
	}

	/**
	 * @brief Parses the configuration file.
	 * @param str The configuration file as a string.
	 * @param layer The layer of the configuration file in our current pos.
	 * @param i The current position in the configuration file.
	 * @param lineCount The current line number.
	 * @return 0 if successful, 1 if error.
	 */
	int ConfigFileParser::parseConfigFile(std::string& str, int layer, unsigned long& i, int& lineCount) {
		std::string line;
		int findQuotesFlag = 0;
		for (; i < str.size(); i++) {
			if (str[i] == '\n') {
				lineCount++;
				if (line.empty() || line.find_first_not_of(' ') == std::string::npos) { // empty line
					line.clear();
					continue;
				} else if (line.find('}') !=
						   std::string::npos) { // closing bracket
					if (line.find_first_not_of(" }") != std::string::npos) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Quotes must be in an otherwise empty line"
								<< std::endl,
							1)
						return 1;
					}
					if (line.find('}') != line.find_last_of('}')) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Multiple brackets on the same line"
								<< std::endl,
							1)
						return 1;
					}
					if (layer == 0) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Unexpected closing bracket found" << std::endl,
							1)
						return 1;
					} else {
						return 0;
					}
				} else if (line.find('{') !=
						   std::string::npos) { // opening bracket
					if (line.find_first_not_of(" {") != std::string::npos) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Quotes must be in an otherwise empty line"
								<< std::endl,
							1)
						return 1;
					}
					if (line.find('{') != line.find_last_of('{')) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Multiple brackets on the same line"
								<< std::endl,
							1)
						return 1;
					}
					if (findQuotesFlag == 0) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Quotes not expected at this point in file"
								<< std::endl,
							1)
						return 1;
					} else {
						findQuotesFlag = 0;
					}
					i++;
					if (parseConfigFile(str, layer + 1, i, lineCount) == 1) {
						return 1;
					}
				} else { // prompt
					if (findQuotesFlag == 1) {
						LOG("Configuration file (line "
								<< lineCount << "): "
								<< "Missing semicolon at the end of line"
								<< std::endl,
							1)
						return 1;
					}
					int ret = handlePrompt(line, layer, lineCount);
					if (ret == 2) {
						return 1;
					}
					findQuotesFlag = ret;
				}
				line.clear();
			} else {
				line += str[i];
			}
		}
		return 0;
	}

	/**
	 * @brief Loads the configuration file, saving all the information.
	 * @param configFileName The name of the configuration file.
	 * @return 0 if successful, 1 if error.
	 */
	int ConfigFileParser::loadConfigFile(std::string& configFileName) {
		std::string file;
		readConfigFile(configFileName, file);
		if (file.empty()) {
			return 1;
		}
		unsigned long i = 0;
		int lineCount = 0;
		if (parseConfigFile(file, 0, i, lineCount) == 1) {
			m_configData.servers.clear();
			return 1;
		}
		if (m_configData.servers.size() == 0) {
			LOG("Configuration file (line "
					<< lineCount << "): "
					<< "No servers found in configuration file" << std::endl,
				1)
			return 1;
		}
		m_isLoaded = 1;
		return 0;
	}

	int ConfigFileParser::getIsLoaded() const {
		return m_isLoaded;
	}

	t_config_data ConfigFileParser::getConfigData() const {
		return m_configData;
	}

	int ConfigFileParser::getServerSize() const {
		return m_configData.servers.size();
	}

	int ConfigFileParser::getServerPort(int index) const {
		return m_configData.servers[index].port;
	}

	uint32_t ConfigFileParser::getServerIp(int index) const {
		return m_configData.servers[index].ip_address;
	}

	std::vector<std::string> ConfigFileParser::getServerNames(int index) const {
		return m_configData.servers[index].server_names;
	}

	std::map<int, std::string> ConfigFileParser::getErrorPages(int index) const {
		return m_configData.servers[index].errorPages;
	}

	unsigned long ConfigFileParser::getMaxBodySize(int index) const {
		return m_configData.servers[index].max_body_size;
	}

	t_server* ConfigFileParser::getServer(int index) {
		return &m_configData.servers[index];
	}

} // namespace config
