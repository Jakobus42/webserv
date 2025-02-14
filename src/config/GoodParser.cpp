#include "config/GoodParser.hpp"

#include "shared/stringUtils.hpp"

namespace config {

	parse_exception::parse_exception(std::size_t line)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed, good luck finding out why") {}

	parse_exception::parse_exception(std::size_t line, const std::string& message)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed: " + message) {}

	parse_exception::~parse_exception() throw() {}

	/**
	 * @brief Constructs a new GoodParser object.
	 */
	GoodParser::GoodParser()
		: m_configs()
		, m_depth(0) {}

	/**
	 * @brief Destroys the GoodParser object.
	 */
	GoodParser::~GoodParser() {}

	/**
	 * @brief Copy constructor.
	 * @param other The other GoodParser object to copy.
	 */
	GoodParser::GoodParser(const GoodParser& other)
		: m_configs(other.m_configs) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other GoodParser object to assign from.
	 * @return A reference to the assigned GoodParser object.
	 */
	GoodParser& GoodParser::operator=(const GoodParser& rhs) {
		if (this == &rhs)
			return *this;
		m_configs = rhs.m_configs;
		return *this;
	}

	// -------------------------------------------------------------------------

	bool GoodParser::parseFile(const std::string& fileName) {
		std::string fileContent;
		std::ifstream file(fileName.c_str());
		if (!file.is_open()) {
			return false;
		}
		char c;
		while (file >> std::noskipws >> c) {
			fileContent.push_back(c);
		}
		file.close();
		if (file.fail()) {
			std::cout << "Oh, crud." << std::endl;
			return false;
		}
		try {
			parseFromString(fileContent);
		} catch (const parse_exception& e) {
			std::cout << e.getMessage() << std::endl;
			return false;
		}
		return true;
	}

	void GoodParser::parseFromString(const std::string& fileContent) throw(config::parse_exception) {
		std::size_t lineIndex = 1;
		{
			std::string currentLine;
			static const std::string WHITESPACE = "\t\r\v\f";
			for (std::string::const_iterator c = fileContent.begin(); c != fileContent.end(); ++c) {
				if (*c == '\n') {
					parseLine(currentLine, lineIndex);
					currentLine.clear();
					lineIndex++;
				} else if (WHITESPACE.find_first_of(*c) == WHITESPACE.size()) {
					currentLine += *c;
				}
			}
		}
	}

	void GoodParser::parseLine(const std::string& line, std::size_t lineIndex) throw(config::parse_exception) {
		if (line.empty() || line.find_first_not_of(' ') == std::string::npos) {
			return;
		}
		if (line.at(line.find_first_not_of(' ')) == '#') {
			return;
		}
	}

} /* namespace config */
