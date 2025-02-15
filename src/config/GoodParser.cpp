#include "config/GoodParser.hpp"

#include "http/Router.hpp"
#include "shared/stringUtils.hpp"

namespace config {

	const std::string GoodParser::WHITESPACE = "\t\r\v\f ";

	parse_exception::parse_exception(std::size_t line)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed, good luck finding out why")
		, m_message("Line " + shared::string::fromNum(line) + ": Parsing failed, good luck finding out why") {}

	parse_exception::parse_exception(const std::string& message)
		: std::runtime_error("Parsing failed: " + message)
		, m_message("Parsing failed: " + message) {}

	parse_exception::parse_exception(std::size_t line, const std::string& message)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed: " + message)
		, m_message("Line " + shared::string::fromNum(line) + ": Parsing failed: " + message) {}

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
		, m_readPos(0) {}

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
		, m_readPos(other.m_readPos) {}

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
		m_readPos = rhs.m_readPos;
		return *this;
	}

	// ------------------------  utility  ----------------------------------- //

	void GoodParser::skipWhitespace() {
		while (m_readPos < m_data.size()) {
			char c = m_data[m_readPos];
			if (c == '\n') {
				++m_lineIndex; // if char is newline, increment lineIndex
				++m_readPos;   // then skip it
			} else if (WHITESPACE.find(c) != std::string::npos) {
				++m_readPos; // if char is whitespace, skip it
			} else {
				break; // no more whitespace
			}
		}
	}

	void GoodParser::consume(std::size_t amount) {
		if (amount > m_readPos + m_data.size()) {
			throw std::runtime_error("GoodParser: Cannot consume more data than available");
		}
		m_readPos += amount;
	}

	// ------------------------  tokens and stuff  -------------------------- //

	// CONSUMES IF MATCHED
	bool GoodParser::matchToken(const std::string& token) {
		skipWhitespace();
		if (m_data.compare(m_readPos, token.size(), token) == 0) {
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
		std::size_t i = 0;
		skipWhitespace();
		while (m_readPos + i < m_data.size() && shouldReadToken(m_data[m_readPos + i])) {
			token.push_back(m_data[m_readPos + i]);
			i++;
		}
		return token;
	}

	inline static bool shouldReadValue(char c) {
		return (c != '\n' && c != ';' && c != '{' && c != '}');
	}

	// read the value until newline, semicolon or either brace is encountered
	// braces probably warrant an exception, though readValue shouldn't handle this
	// expecting semicolons at the end of each value is probably sensible though
	// CONSUMES WHAT IT READS
	std::string GoodParser::readValue() {
		std::string value = "";
		std::cout << m_data[m_readPos] << std::endl;
		skipWhitespace();
		while (m_readPos < m_data.size() && shouldReadValue(m_data[m_readPos])) {
			value.push_back(m_data[m_readPos]);
			consume(1);
		}
		if (m_readPos >= m_data.size() || m_data[m_readPos] != ';') {
			throw parse_exception(m_lineIndex, "Expected semicolon after value");
		}
		consume(1);
		return value;
	}

	CommandType GoodParser::matchDirective(const std::string& token, const std::map<std::string, CommandType>& expectedDirectives) {
		std::map<std::string, CommandType>::const_iterator matchedDirective = expectedDirectives.find(token);
		if (matchedDirective == expectedDirectives.end()) {
			return _D_NOT_VALID;
		}
		m_readPos += token.size();
		return matchedDirective->second;
	}

	const std::map<std::string, CommandType>& GoodParser::locationDirectives() {
		static std::map<std::string, CommandType> allowedDirectives;

		if (allowedDirectives.empty()) {
			allowedDirectives["root"] = D_ROOT;
			allowedDirectives["return"] = D_RETURN;
			allowedDirectives["limit_except"] = D_LIMIT_EXCEPT;
			allowedDirectives["upload_dir"] = D_UPLOAD_DIR;
			allowedDirectives["index"] = D_INDEX;
			allowedDirectives["autoindex"] = D_AUTOINDEX;
			allowedDirectives["location"] = D_LOCATION;
		}
		return allowedDirectives;
	}

	const std::map<std::string, CommandType>& GoodParser::serverDirectives() {
		static std::map<std::string, CommandType> allowedDirectives;

		if (allowedDirectives.empty()) {
			allowedDirectives["listen"] = D_LISTEN;
			allowedDirectives["client_max_body_size"] = D_CLIENT_MAX_BODY_SIZE;
			allowedDirectives["data_dir"] = D_DATA_DIR;
			allowedDirectives["server_name"] = D_SERVER_NAME;
			allowedDirectives["root"] = D_ROOT;
			allowedDirectives["return"] = D_RETURN;
			allowedDirectives["limit_except"] = D_LIMIT_EXCEPT;
			allowedDirectives["upload_dir"] = D_UPLOAD_DIR;
			allowedDirectives["index"] = D_INDEX;
			allowedDirectives["autoindex"] = D_AUTOINDEX;
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
			if (c == '#') {
				// skip until '\n'
				while (file >> std::noskipws >> c && c != '\n') {}
			}
			m_data.push_back(c);
		}
		std::cout << m_data << std::endl;
		// file.close();
		// if (file.fail()) {
		// 	std::cout << "Oh, crud." << std::endl;
		// 	return false;
		// }
		try {
			parseFromData();
		} catch (const parse_exception& e) {
			std::cout << e.getMessage() << std::endl;
			return false;
		}
		return true;
	}

	void GoodParser::parseFromData() throw(config::parse_exception) {
		while (m_readPos < m_data.size()) {
			skipWhitespace();
			expectServerBlock();
		}
	}

	// ------------------------  block parsing ------------------------------ //

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
	 * - autoindex (location.autoindex)
	 * - location (location.locations) (can exist more than once)
	 */
	void GoodParser::expectServerBlock() throw(parse_exception) {
		if (!matchToken("server")) {
			throw parse_exception(m_lineIndex, "Expected 'server' keyword");
		}
		skipWhitespace();
		if (m_readPos >= m_data.size() || m_data[m_readPos] != '{') {
			throw parse_exception(m_lineIndex, "Expected opening brace after Server");
		}
		consume(1); // consume '{'
		m_depth++;

		Server thisServer;

		while (m_readPos < m_data.size()) {
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '{') {
				throw parse_exception(m_lineIndex, "Unexpected opening brace in Server block");
			}
			if (m_readPos < m_data.size() && m_data[m_readPos] == '}') {
				consume(1); // consume '}'
				m_depth--;
				thisServer.validate();
				m_configs.push_back(thisServer);
				return;
			}
			std::string token = readToken();
			if (matchToken("location")) {
				expectLocationBlock(thisServer.location);
			} else if (!token.empty()) {
				CommandType type = matchDirective(token, serverDirectives());
				if (type == _D_NOT_VALID) {
					throw parse_exception(m_lineIndex, "Unexpected directive in Server block: " + token);
				}
				std::string value = readValue();
				setServerValue(value, type, thisServer);
			}
		}
		throw parse_exception(m_lineIndex, "Server block not closed with '}'");
	}

	static bool isValidPath(const std::string& path) {
		return !path.empty() && path.at(0) == '/' && path.size() <= PATH_MAX;
	}

	/**
	 * Possible Location directives are:
	 * - root
	 * - return (redirectUri)
	 * - limit_except (allowedMethods)
	 * - upload_dir (uploadSubdirectory)
	 * - index (indexFile)
	 * - autoindex
	 * - location (locations) (can exist more than once)
	 */
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
		if (m_readPos >= m_data.size() || m_data[m_readPos] != '{') {
			throw parse_exception(m_lineIndex, "Expected '{' after location path");
		}
		consume(1); // consume '{'
		m_depth++;

		Location thisLocation;

		thisLocation.path = http::Router::splitPath(path);
		while (m_readPos <= m_data.size()) {
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '{') {
				throw parse_exception(m_lineIndex, "Unexpected opening brace in location block");
			}
			if (m_readPos < m_data.size() && m_data[m_readPos] == '}') {
				consume(1); // consume '}'
				m_depth--;
				thisLocation.validate();
				// validate Location whether enough stuff is present
				// is anything mandatory for a location?
				parentLocation.locations.push_back(thisLocation);
				return;
			}
			std::string token = readToken();
			if (matchToken("location")) {
				expectLocationBlock(thisLocation);
			} else if (!token.empty()) {
				// process location directives
				CommandType type = matchDirective(token, locationDirectives());
				if (type == _D_NOT_VALID) {
					throw parse_exception(m_lineIndex, "Unexpected directive in Location block: " + token);
				}
				std::string value = readValue();
				setLocationValue(value, type, thisLocation);
			}
		}
		throw parse_exception(m_lineIndex, "Location block not closed with '}");
	}

	void GoodParser::setServerValue(const std::string& value, CommandType type, Server& server) {
		static std::map<CommandType, ServerTokenParser> tokenParsers;

		if (type > _D_SERVER_TYPES) {
			return setLocationValue(value, type, server.location);
		}
		if (tokenParsers.empty()) {
			tokenParsers[D_LISTEN] = &config::GoodParser::parseListen;
			tokenParsers[D_CLIENT_MAX_BODY_SIZE] = &config::GoodParser::parseClientMaxBodySize;
			tokenParsers[D_DATA_DIR] = &config::GoodParser::parseDataDir;
			tokenParsers[D_SERVER_NAME] = &config::GoodParser::parseServerName;
		}

		(this->*(tokenParsers[type]))(value, server);
	}

	// cppcheck-suppress constParameter
	void GoodParser::setLocationValue(const std::string& value, CommandType type, Location& location) {
		static std::map<CommandType, LocationTokenParser> tokenParsers;

		if (tokenParsers.empty()) {
			tokenParsers[D_ROOT] = &config::GoodParser::parseRoot;
			tokenParsers[D_RETURN] = &config::GoodParser::parseReturn;
			tokenParsers[D_LIMIT_EXCEPT] = &config::GoodParser::parseLimitExcept;
			tokenParsers[D_UPLOAD_DIR] = &config::GoodParser::parseUploadDir;
			tokenParsers[D_INDEX] = &config::GoodParser::parseIndex;
			tokenParsers[D_AUTOINDEX] = &config::GoodParser::parseAutoindex;
			// tokenParsers[D_LOCATION] = &config::GoodParser::parseLocation;
		}

		(this->*(tokenParsers[type]))(value, location);
	}

	// ------------------------  server parsers  ---------------------------- //

	void GoodParser::parseListen(const std::string& value, Server& server) {
		(void)value;
		(void)server;
	}

	void GoodParser::parseClientMaxBodySize(const std::string& value, Server& server) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> server.maxBodySize)) {
			throw parse_exception(m_lineIndex, "Invalid value for client_max_body_size");
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in client_max_body_size: " + token);
		}
	}

	void GoodParser::parseDataDir(const std::string& value, Server& server) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for data_dir: " + value);
		}
		server.dataDirectory = value; // check whether this needs to be stripped of whitespace
	}

	void GoodParser::parseServerName(const std::string& value, Server& server) {
		std::vector<std::string> splitNames;
		std::stringstream ss(value);
		std::string name;

		while (ss >> name) {
			splitNames.push_back(name);
		}
		if (splitNames.empty()) {
			throw parse_exception(m_lineIndex, "Expected value for server_name");
		}
		server.serverNames = splitNames;
	}

	// ------------------------  location parsers  -------------------------- //

	void GoodParser::parseRoot(const std::string& value, Location& location) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for root: " + value);
		}
		location.root = http::Router::splitPath(value);
	}

	void GoodParser::parseReturn(const std::string& value, Location& location) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for return: " + value);
		}
		location.path = http::Router::splitPath(value); // ensure this won't throw
	}

	// if no limit_except is present, default initialization
	// for locations should permit all methods; GET POST and DELETE
	void GoodParser::parseLimitExcept(const std::string& value, Location& location) {
		static std::map<std::string, http::Method> implementedMethods;

		if (implementedMethods.empty()) {
			implementedMethods["GET"] = http::GET;
			implementedMethods["POST"] = http::POST;
			implementedMethods["DELETE"] = http::DELETE;
		}

		std::set<http::Method> allowedMethods;
		std::stringstream ss(value);
		std::string token;

		while (ss >> token) {
			if (implementedMethods.find(token) == implementedMethods.end()) {
				throw parse_exception(m_lineIndex, "Unexpected method for limit_except: " + token);
			}
			if (allowedMethods.find(implementedMethods[token]) != allowedMethods.end()) {
				throw parse_exception(m_lineIndex, "Duplicte method for limit_except: " + token);
			}
			allowedMethods.insert(implementedMethods[token]);
		}
		if (allowedMethods.empty()) {
			throw parse_exception(m_lineIndex, "Expected value for limit_except");
		}
		location.allowedMethods = allowedMethods;
	}

	void GoodParser::parseUploadDir(const std::string& value, Location& location) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for upload_dir: " + value);
		}
		location.uploadSubdirectory = value; // strip this of whitespace if that doesn't happen yet
	}

	void GoodParser::parseIndex(const std::string& value, Location& location) {
		std::vector<std::string> indexFiles;
		std::stringstream ss(value);
		std::string token;

		while (ss >> token) {
			indexFiles.push_back(token);
		}
		std::cout << "parseIndex parsing: " << value << std::endl;
		if (indexFiles.empty()) {
			throw parse_exception(m_lineIndex, "Expected value for index");
		}
		location.indexFile = indexFiles;
	}

	void GoodParser::parseAutoindex(const std::string& value, Location& location) {
		static std::set<std::string> allowedValues;

		if (allowedValues.empty()) {
			allowedValues.insert("on");
			allowedValues.insert("off");
		}
		// can only be "on" or "off", everything else is invalid
		std::stringstream ss(value);
		std::string token;

		ss >> token;
		if (allowedValues.find(token) == allowedValues.end()) {
			throw parse_exception(m_lineIndex, "Unexpected value for autoindex: " + value);
		}
		if (token == "on") {
			location.autoindex = true;
		} else if (token == "off") {
			location.autoindex = false;
		}
		if (ss >> token && !token.empty()) {
			throw parse_exception(m_lineIndex, "Unexpected token in autoindex: " + token);
		}
	}

	// void GoodParser::parseLocation(const std::string& value, Location& location) {

	// 	(void)value;
	// 	(void)location;
	// 	std::cout << "parseLocation parsing: " << value << std::endl;
	// }

} /* namespace config */
