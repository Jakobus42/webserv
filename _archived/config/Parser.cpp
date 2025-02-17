#include "config/Parser.hpp"

namespace config {

	/**
	 * @brief Constructs a new Parser object.
	 */
	Parser::Parser() {
		m_isLoaded = 0;
	}

	/**
	 * @brief Destroys the Parser object.
	 */
	Parser::~Parser() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Parser object to copy.
	 */
	Parser::Parser(const Parser&) {
		m_isLoaded = 0;
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Parser object to assign from.
	 * @return A reference to the assigned Parser object.
	 */
	Parser& Parser::operator=(const Parser&) {
		m_isLoaded = 0;
		return *this;
	}

	/**
	 * @brief Reads the configuration file.
	 * @param fileName The name of the configuration file.
	 * @param line the location where the configuration file is stored by the read.
	 */
	int Parser::readFile(std::string& fileName, std::string& line) {
		if (fileName.empty()) {
			std::cout << "Configuration file name is empty." << std::endl;
			return 1;
		}
		std::ifstream infile;
		infile.open(fileName.c_str());
		char c;
		if (!infile.is_open()) {
			std::cout << "Could not open configuration file." << std::endl;
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
			std::cout << "Could not close Configuration file" << std::endl;
			return 1;
		}
		if (line.empty()) {
			std::cout << "Configuration file is empty." << std::endl;
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
	int Parser::parseFile(std::string& str, int layer, unsigned long& i, int& lineCount) {
		std::string line;
		int findQuotesFlag = 0;

		for (; i < str.size(); i++) {
			if (str[i] != '\n') {
				line += str[i];
			} else {
				lineCount++;
				if (line.empty() || line.find_first_not_of(' ') == std::string::npos) { // empty line
					line.clear();
					continue;
				}
				if (line.find('}') != std::string::npos) { // closing bracket
					if (line.find_first_not_of(" }") != std::string::npos) {
						return genericError(lineCount, "Quotes must be in an otherwise empty line");
					}
					if (line.find('}') != line.find_last_of('}')) {
						return genericError(lineCount, "Multiple brackets on the same line");
					}
					if (layer == 0) {
						return genericError(lineCount, "Unexpected closing bracket found");
					}
					if (findQuotesFlag == 1) {
						return genericError(lineCount, "Missing semicolon at the end of line");
					}
					return 0;
				}
				if (line.find('{') != std::string::npos) { // opening bracket
					if (line.find_first_not_of(" {") != std::string::npos) {
						return genericError(lineCount, "Quotes must be in an otherwise empty line");
					}
					if (line.find('{') != line.find_last_of('{')) {
						return genericError(lineCount, "Multiple brackets on the same line");
					}
					if (findQuotesFlag == 0) {
						return genericError(lineCount, "Quotes not expected at this point in file");
					}
					findQuotesFlag = 0;
					i++;
					if (parseFile(str, layer + 1, i, lineCount) == 1) {
						return 1;
					}
				} else { // prompt
					if (findQuotesFlag == 1) {
						return genericError(lineCount, "Missing semicolon at the end of line");
					}
					int ret = handlePrompt(line, layer, lineCount);
					if (ret == 2) {
						return 1;
					}
					findQuotesFlag = ret;
				}
				line.clear();
			}
		}
		return 0;
	}

	/**
	 * @brief Loads the configuration file, saving all the information.
	 * @param fileName The name of the configuration file.
	 * @return 0 if successful, 1 if error.
	 */
	int Parser::loadConfig(std::string& fileName) {
		std::string file;
		readFile(fileName, file);
		if (file.empty()) {
			return 1;
		}
		unsigned long i = 0;
		int lineCount = 0;
		if (parseFile(file, 0, i, lineCount) == 1) {
			m_serverConfigs.clear();
			return 1;
		}
		if (m_serverConfigs.empty()) {
			return genericError(lineCount, "No servers found in configuration file");
		}
		for (std::size_t index = 0; index < m_serverConfigs.size(); ++index) {
			if (!m_serverConfigs[index].hasGlobalRoot()) {
				std::cout << "Configuration error: Server block " << index + 1
						  << " missing mandatory global_root directive" << std::endl;
				return 1;
			}
			if (!m_serverConfigs[index].hasDataDir()) {
				std::cout << "Configuration error: Server block " << index + 1
						  << " missing mandatory data_dir directive" << std::endl;
				return 1;
			}
			m_serverConfigs[index].globalRoot.locations = m_serverConfigs[index].locations;
		}
		m_isLoaded = 1;
		return 0;
	}

	int Parser::getIsLoaded() const {
		return m_isLoaded;
	}

	const std::vector<config::ServerConfig>& Parser::getServerConfigs() const {
		return m_serverConfigs;
	}

	int Parser::getServerSize() const {
		return m_serverConfigs.size();
	}

	int Parser::getServerPort(int index) const {
		return m_serverConfigs[index].port;
	}

	uint32_t Parser::getServerIp(int index) const {
		return m_serverConfigs[index].ip_address;
	}

	std::vector<std::string> Parser::getServerNames(int index) const {
		return m_serverConfigs[index].server_names;
	}

	std::map<int, std::string> Parser::getErrorPages(int index) const {
		return m_serverConfigs[index].errorPages;
	}

	unsigned long Parser::getMaxBodySize(int index) const {
		return m_serverConfigs[index].max_body_size;
	}

	ServerConfig* Parser::getServerConfig(int index) {
		return &m_serverConfigs[index];
	}

} // namespace config
