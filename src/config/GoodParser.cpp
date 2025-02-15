#include "config/GoodParser.hpp"

#include "http/Router.hpp"
#include "shared/stringUtils.hpp"

namespace config {

	const std::string GoodParser::WHITESPACE = "\t\r\v\f";

	parse_exception::parse_exception(std::size_t line)
		: std::runtime_error("Line " + shared::string::fromNum(line) + ": Parsing failed, good luck finding out why")
		, m_message("Line " + shared::string::fromNum(line) + ": Parsing failed, good luck finding out why") {}

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
		std::cout << "skipWhitespace()" << std::endl;
		while (m_readPos < m_data.size()) {
			char c = m_data[m_readPos];
			if (c == '\n') {
				++m_lineIndex; // if char is newline, increment lineIndex
				++m_readPos;   // then skip it
				std::cout << "Newline!" << std::endl;
			} else if (WHITESPACE.find(c) != std::string::npos) {
				std::cout << "'" << c << "' is whitespace!" << std::endl;
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
		std::cout << "matchToken()" << std::endl;
		skipWhitespace();
		if (m_data.compare(m_readPos, token.size(), token) == 0) {
			consume(token.size());
			std::cout << "matched " << token << "!" << std::endl;
			return true;
		}
		return false;
	}

	inline static bool shouldReadToken(char c) {
		return !std::isspace(static_cast<unsigned char>(c)) && c != '{' && c != '}';
	}

	// DOES NOT CONSUME
	std::string GoodParser::readToken() {
		std::cout << "readToken()" << std::endl;
		std::string token = "";
		std::size_t i = 0;
		skipWhitespace();
		while (m_readPos + i < m_data.size() && shouldReadToken(m_data[m_readPos + i])) {
			token.push_back(m_data[m_readPos + i]);
			i++;
		}
		std::cout << "read " << token << "!" << std::endl;
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
		std::cout << "readValue()" << std::endl;
		std::string value = "";
		std::cout << m_data[m_readPos] << std::endl;
		skipWhitespace();
		while (m_readPos < m_data.size() && shouldReadValue(m_data[m_readPos])) {
			std::cout << m_data[m_readPos];
			value.push_back(m_data[m_readPos]);
			consume(1);
		}
		std::cout << std::endl;
		if (m_readPos >= m_data.size() || m_data[m_readPos] != ';') {
			throw parse_exception(m_lineIndex, "Expected semicolon after value");
		}
		std::cout << "read " << value << "!" << std::endl;
		consume(1);
		return value;
	}

	CommandType GoodParser::matchDirective(const std::string& token, const std::map<std::string, CommandType>& expectedDirectives) {
		std::cout << "matchDirective()" << std::endl;
		std::map<std::string, CommandType>::const_iterator matchedDirective = expectedDirectives.find(token);
		if (matchedDirective == expectedDirectives.end()) {
			return _D_NOT_VALID;
		}
		m_readPos += token.size();
		std::cout << "matched directive " << token << "!" << std::endl;
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
			allowedDirectives["location"] = D_LOCATION;
		}
		return allowedDirectives;
	}

	const std::map<std::string, CommandType>& GoodParser::serverDirectives() {
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
			if (c == '#') {
				// skip until '\n'
				while (file >> std::noskipws >> c && c != '\n') {}
			}
			m_data.push_back(c);
		}
		std::cout << m_data << std::endl;
		// strip out everything between '#' and the next '\n'
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
		// parse out comments
		// std::string currentLine;
		while (m_readPos < m_data.size()) {
			std::cout << "expecting server block..." << std::endl;
			skipWhitespace();
			expectServerBlock();
			std::cout << "readPos now " << m_readPos << std::endl;
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
			std::cout << "exServBlock loop" << std::endl;
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '}') {
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
				CommandType type = matchDirective(token, serverDirectives());
				if (type == _D_NOT_VALID) {
					throw parse_exception(m_lineIndex, "Unexpected directive in Server block: " + token);
				}
				std::string value = readValue();
				// process server directives
				// if the token doesn't match any expected directives,
				// throw exception
				std::cout << "Value for token '" << token << "': '" << value << "' (in server block)" << std::endl;
			}
		}
		throw parse_exception(m_lineIndex, "Server block not closed with '}'");
	}

	static bool isValidPath(const std::string& path) {
		return path.at(0) == '/';
	}

	/**
	 * Possible Location directives are:
	 * - root
	 * - return (redirectUri)
	 * - limit_except (allowedMethods)
	 * - upload_dir (uploadSubdirectory)
	 * - index (indexFile)
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
			std::cout << "exLocBlock loop" << std::endl;
			skipWhitespace();
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
				std::cout << "Value for token '" << token << "': '" << value << "'" << std::endl;
			}
		}
		throw parse_exception(m_lineIndex, "Location block not closed with '}");
	}

} /* namespace config */
