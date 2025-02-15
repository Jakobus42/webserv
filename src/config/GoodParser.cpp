#include "config/GoodParser.hpp"

#include "http/Router.hpp"
#include "shared/stringUtils.hpp"

namespace config {

	const std::string GoodParser::WHITESPACE = "\t\r\v\f";

	parse_exception::parse_exception(std::size_t line)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed, good luck finding out why") {}

	parse_exception::parse_exception(std::size_t line, const std::string& message)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed: " + message) {}

	parse_exception::~parse_exception() throw() {}

	const std::string& parse_exception::getMessage() const { return m_message; }

	/**
	 * @brief Constructs a new GoodParser object.
	 */
	GoodParser::GoodParser()
		: m_data("")
		, m_configs()
		, m_depth(0)
		, m_lineIndex(1)
		, m_writePos(0) {}

	/**
	 * @brief Destroys the GoodParser object.
	 */
	GoodParser::~GoodParser() {}

	/**
	 * @brief Copy constructor.
	 * @param other The other GoodParser object to copy.
	 */
	GoodParser::GoodParser(const GoodParser& other)
		: m_data(other.m_data)
		, m_configs(other.m_configs)
		, m_depth(other.m_depth)
		, m_lineIndex(other.m_lineIndex)
		, m_writePos(other.m_writePos) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other GoodParser object to assign from.
	 * @return A reference to the assigned GoodParser object.
	 */
	GoodParser& GoodParser::operator=(const GoodParser& rhs) {
		if (this == &rhs)
			return *this;
		m_data = rhs.m_data;
		m_configs = rhs.m_configs;
		m_depth = rhs.m_depth;
		m_lineIndex = rhs.m_lineIndex;
		m_writePos = rhs.m_writePos;
		return *this;
	}

	// ------------------------  utility  ----------------------------------- //

	void GoodParser::skipWhitespace() {
		while (m_writePos < m_data.size()) {
			char c = m_data[m_writePos];
			if (c == '\n') {
				++m_lineIndex; // if char is newline, increment lineIndex
				++m_writePos;  // then skip it
			} else if (WHITESPACE.find(c) != std::string::npos) {
				++m_writePos; // if char is whitespace, skip it
			} else {
				break; // no more whitespace
			}
		}
	}

	void GoodParser::consume(std::size_t amount) {
		if (amount > m_writePos + m_data.size()) {
			throw std::runtime_error("GoodParser: Cannot consume more data than available");
		}
		m_writePos += amount;
	}

	// ------------------------  tokens and stuff  -------------------------- //

	// CONSUMES IF MATCHED
	bool GoodParser::matchToken(const std::string& token) {
		skipWhitespace();
		if (m_data.compare(m_writePos, token.size(), token) == 0) {
			consume(token.size());
			return true;
		}
		return false;
	}

	inline static bool shouldReadToken(char c) {
		return !std::isspace(static_cast<unsigned char>(c)) && c != '{' && c != '}';
	}

	// DOES NOT CONSUME
	std::string GoodParser::readToken() {
		std::string token = "";
		skipWhitespace();
		while (m_writePos < m_data.size() && shouldReadToken(m_data[m_writePos])) {
			token.push_back(m_data[m_writePos]);
		}
		return token;
	}

	inline static bool shouldReadValue(char c) {
		return c != '\n' && c != ';' && c != '{' && c != '}';
	}

	// read the value until newline, semicolon or either brace is encountered
	// braces probably warrant an exception, though readValue shouldn't handle this
	// expecting semicolons at the end of each value is probably sensible though
	std::string GoodParser::readValue() {
		std::string value = "";
		skipWhitespace();
		while (m_writePos < m_data.size() && shouldReadValue(m_data[m_writePos])) {
			value.push_back(m_data[m_writePos]);
			consume(1);
		}
		if (m_writePos >= m_data.size() || m_data[m_writePos] != ';') {
			throw parse_exception(m_lineIndex, "Expected semicolon after value");
		}
		return value;
	}

	const std::map<std::string, CommandType>& locationDirectives() {
		static std::map<std::string, CommandType> allowedDirectives;

		if (allowedDirectives.empty()) {
			allowedDirectives["root"] = D_ROOT;
			allowedDirectives["return"] = D_RETURN;
			allowedDirectives["limit_except"] = D_LIMIT_EXCEPT;
			allowedDirectives["upload_dir"] = D_UPLOAD_DIR;
			allowedDirectives["index"] = D_INDEX;
			allowedDirectives["location"] = D_LOCATION;
		}
		return allowedDirectives;
	}

	const std::map<std::string, CommandType>& serverDirectives() {
		static std::map<std::string, CommandType> allowedDirectives;

		if (allowedDirectives.empty()) {
			allowedDirectives["port"] = D_PORT;
			allowedDirectives["listen"] = D_LISTEN;
			allowedDirectives["client_max_body_size"] = D_CLIENT_MAX_BODY_SIZE;
			allowedDirectives["data_dir"] = D_DATA_DIR;
			allowedDirectives["server_name"] = D_SERVER_NAME;
			allowedDirectives["root"] = D_ROOT;
			allowedDirectives["return"] = D_RETURN;
			allowedDirectives["limit_except"] = D_LIMIT_EXCEPT;
			allowedDirectives["upload_dir"] = D_UPLOAD_DIR;
			allowedDirectives["index"] = D_INDEX;
			allowedDirectives["location"] = D_LOCATION;
		}
		return allowedDirectives;
	}

	// ------------------------  parsing infrastructure  -------------------- //

	bool GoodParser::parseFile(const std::string& fileName) {
		std::ifstream file(fileName.c_str());
		if (!file.is_open()) {
			return false;
		}
		char c;
		while (file >> std::noskipws >> c) {
			m_data.push_back(c);
		}
		file.close();
		if (file.fail()) {
			std::cout << "Oh, crud." << std::endl;
			return false;
		}
		try {
			parseFromData();
		} catch (const parse_exception& e) {
			std::cout << e.getMessage() << std::endl;
			return false;
		}
		return true;
	}

	void GoodParser::parseFromData() throw(config::parse_exception) {
		// std::string currentLine;
		while (m_writePos < m_data.size()) {
			expectServerBlock();
		}
		// for (std::string::const_iterator c = m_data.begin(); c != m_data.end(); ++c) {
		// 	if (*c == '\n') {
		// 		// parseLine(currentLine);
		// 		currentLine.clear();
		// 		m_lineIndex++;
		// 	} else if (WHITESPACE.find_first_of(*c) == WHITESPACE.size()) {
		// 		currentLine += *c;
		// 	}
		// }
	}

	// ------------------------  block parsing ------------------------------ //

	void GoodParser::expectServerBlock() throw(parse_exception) {
		if (!matchToken("server")) {
			throw parse_exception(m_lineIndex, "Expected 'server' keyword");
		}
		skipWhitespace();
		if (m_writePos >= m_data.size() || m_data[m_writePos] != '{') {
			throw parse_exception(m_lineIndex, "Expected opening brace after Server");
		}
		consume(1); // consume '{'
		m_depth++;

		Server thisServer;

		while (m_writePos < m_data.size()) {
			skipWhitespace();
			if (m_writePos < m_data.size() && m_data[m_writePos] == '}') {
				consume(1); // consume '}'
				m_depth--;
				// validate server for mandatory keys
				thisServer.validate();
				// if valid, push to server configs
				// then, move on to parsing the next one
				m_configs.push_back(thisServer);
				return;
			}
			std::string token = readToken();
			if (matchToken("location")) {
				expectLocationBlock(thisServer.location);
			} else if (!token.empty()) {
				// process server directives
				// if the token doesn't match any expected directives,
				// throw exception
				/**
				 * Possible Server directives are:
				 * - port
				 * - listen (ipAddress)
				 * - client_max_body_size (maxBodySize)
				 * - data_dir (dataDirectory)
				 * - server_name (serverNames)
				 *
				 * - root (location.root)
				 * - return (location.redirectUri)
				 * - limit_except (location.allowedMethods)
				 * - upload_dir (location.uploadSubdirectory)
				 * - index (location.indexFile)
				 * - location (location.locations) (can exist more than once)
				 */
			}
		}
		throw parse_exception(m_lineIndex, "Server block not closed with '}'");
	}

	static bool isValidPath(const std::string& path) {
		return path.at(0) == '/';
	}

	void GoodParser::expectLocationBlock(Location& parentLocation) throw(parse_exception) {
		std::string path = readToken();

		if (path.empty()) {
			throw parse_exception(m_lineIndex, "Location has no path");
		}
		if (!isValidPath(path)) {
			throw parse_exception(m_lineIndex, "Path is invalid");
		}
		consume(path.length());
		skipWhitespace();
		if (m_writePos >= m_data.size() || m_data[m_writePos] != '{') {
			throw parse_exception(m_lineIndex, "Expected '{' after location path");
		}
		consume(1); // consume '{'
		m_depth++;

		Location thisLocation;

		thisLocation.path = http::Router::splitPath(path);
		while (m_writePos <= m_data.size()) {
			skipWhitespace();
			if (m_writePos < m_data.size() && m_data[m_writePos] == '}') {
				consume(1); // consume '}'
				m_depth--;
				thisLocation.validate();
				// validate Location whether enough stuff is present
				// is anything mandatory for a location?
				// push location to the currentLocation, probably pass this as param?
				parentLocation.locations.push_back(thisLocation);
				return;
			}
			std::string token = readToken();
			if (matchToken("location")) {
				expectLocationBlock(thisLocation);
			} else if (!token.empty()) {
				// process location directives
				/**
				 * Possible Location directives are:
				 * - root
				 * - return (redirectUri)
				 * - limit_except (allowedMethods)
				 * - upload_dir (uploadSubdirectory)
				 * - index (indexFile)
				 * - location (locations) (can exist more than once)
				 */
			}
		}
		throw parse_exception(m_lineIndex, "Location block not closed with '}");
	}

} /* namespace config */
