#include "../../include/configfile/ConfigFileParser.hpp"

namespace configfile {

/**
 * @brief Constructs a new ConfigFileParser object.
 */
ConfigFileParser::ConfigFileParser() {

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
ConfigFileParser::ConfigFileParser(const ConfigFileParser &) {

}

/**
 * @brief Constructs a new ConfigFileParser object.
 * @param configFileName The name of the configuration file.
 */
ConfigFileParser::ConfigFileParser(std::string &configFileName) 
{
	loadConfigFile(configFileName);
}

/**
 * @brief Copy assignment operator.
 * @param other The other ConfigFileParser object to assign from.
 * @return A reference to the assigned ConfigFileParser object.
 */
ConfigFileParser& ConfigFileParser::operator=(const ConfigFileParser &) {
    return *this;
}

/**
 * @brief Reads the configuration file.
 * @param configFileName The name of the configuration file.
 */
 std::string ConfigFileParser::readConfigFile(std::string &configFileName) 
 {
	if (configFileName.empty()) {
		std::cerr << "Error: Configuration file name is empty." << std::endl;
		return std::string();
	}
	std::string line;
	std::ifstream infile;
	infile.open(configFileName.c_str());
	char c;
	if (!infile.is_open())	{
		std::cerr << "Error: Could not open configuration file." << std::endl;
		return std::string();
	}
	while(!infile.eof() && infile >> std::noskipws >> c) {
		if (c == '#') {
			while (c != '\n') {
				infile >> std::noskipws >> c;
			}
			line += '\n';
		}
		else {
			line += c;
		}
	}
	infile.close();
	if (infile.is_open()) {
		std::cerr << "Error: Could not close Configuration file" << std::endl;
	}
	if (line.empty()) {
		std::cerr << "Error: Configuration file is empty." << std::endl;
		return std::string();
	}
	return line;
}

/**
 * @brief parses a line in the configuration file.
 * @param 
 */
int ConfigFileParser::handle_prompt(std::string &line, int layer)
{
	//return 0 if successful and semicolon is found, 1 if semicolon is not found, 2 if error
	//cut off trailing and starting spaces
	int qoute_flag = 1;
	while (line[line.size() - 1] == ' ')
	{
		line.erase(line.size() - 1);
	}
	while (line[0] == ' ')
	{
		line.erase(0, 1);
	}
	if (line[line.size() - 1]== ';')
	{
		qoute_flag = 0;
		line.erase(line.size() - 1);
	}
	if (line.find(';') != std::string::npos)
	{
		std::cerr << "Error: Configuration file: missplaced semicolon (;) found" << std::endl;
		return 2;
	}
	std::vector<std::string> args;
	std::stringstream stream(line);
	std::string key;
	while (getline(stream, key, ' '))
	{
		args.push_back(key);
	}
	//TO-DO: Parse the arguments
	(void)layer;
	if (args.size() > 0)
	{
		std::cout << "cmd: " << args[0] << " args: ";
	}
	if (args.size() > 1)
	{
	for (unsigned long i = 1; i < args.size(); i++)
		{
			std::cout << args[i];
		}
	}
	std::cout << std::endl;
	return qoute_flag;
}



/**
 * @brief Parses the configuration file.
 * @param str The configuration file as a string.
 * @param layer The layer of the configuration file in our current pos.
 * @return 0 if successful, 1 if error.
 */
int ConfigFileParser::parseConfigFile(std::string &str, int layer, unsigned long &i)
{
	std::string line;
	int find_quotes_flag = 0;
	//changing tabs to spaces
	if (layer == 0)
	{
		for (unsigned long b = 0; b < str.size(); b++)
		{
			if (str[b] == '	')
				str[b] = ' ';
		}
	}
	for (;i < str.size(); i++)
	{
		
		if (str[i] == '\n' || i == str.size() - 1)
		{
			std::cout << "hi from layer: " << layer << " i: " << i << std::endl;
			if (line.empty() || line.find_first_not_of(' ') == std::string::npos)
			{
				//std::cout << "empty line" << std::endl;
				line.clear();
				continue;
			}
			else if (line.find('}') != std::string::npos)
			{
				//check if the line contains anything else than brackets or whitespace			
				if (line.find_first_not_of(" }") != std::string::npos)
				{
					std::cerr << "Error: Configuration file: Quotes must be in an otherwise empty line" << std::endl;
					return 1;
				}
				//check if there are multiple brackets on the same line
				if (line.find('}') != line.find_last_of('}'))
				{
					std::cerr << "Error: Configuration file: Multiple brackets on the same line" << std::endl;
					return 1;
				}
				std::cout << "found closing bracket on layer: " << layer << std::endl;
				if (layer == 0)
				{
					std::cerr << "Error: Configuration file: Unexpected closing bracket found" << std::endl;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else if (line.find('{') != std::string::npos)
			{
				//find if the line contains anything else than brackets or whitespace
				if (line.find_first_not_of(" {") != std::string::npos)
				{
					std::cerr << "Error: Configuration file: Quotes must be in an otherwise empty line" << std::endl;
					return 1;
				}
				//check if there are multiple brackets on the same line
				if (line.find('{') != line.find_last_of('{'))
				{
					std::cerr << "Error: Configuration file: Multiple brackets on the same line" << std::endl;
					return 1;
				}
				//find if quotes are expected here
				if (find_quotes_flag == 0)
				{
					std::cerr << "Error: Configuration file: Quotes not expected at this point in file" << std::endl;
					return 1;
				}
				else
				{
					find_quotes_flag = 0;
				}
				if (parseConfigFile(str, layer + 1, i) == 1)
				{
					return 1;
				}
			}
			else
			{
				//std::cout << "found line: " << line << std::endl;
				if (find_quotes_flag == 1)
				{
					std::cerr << "Error: Configuration file: Missing semicolon at the end of line"<< std::endl;
					return 1;
				}
				int ret = handle_prompt(line, layer);
				//std::cout << "ret: " << ret << std::endl;
				if (ret == 2)
				{
					return 1;
				}
				find_quotes_flag = ret;

			}
			line.clear();
		}
		else
		{
			line += str[i];
		}
	}
	return 0;
}

/**
 * @brief Loads the configuration file, saving all the information. Also called by the constructor if string is given.
 * @param configFileName The name of the configuration file.
 */
int ConfigFileParser::loadConfigFile(std::string &configFileName) 
{
	std::string file = readConfigFile(configFileName);
	if (file.empty())
	{
		return 1;
	}
	unsigned long i = 0;
	if (parseConfigFile(file, 0, i) == 1)
	{
		return 1;
	}
	return 0;
}



} /* namespace configfile */
