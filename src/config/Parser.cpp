#include "config/Parser.hpp"

#include <limits.h>
#include <signal.h>

#include "shared/Logger.hpp"
#include "shared/fileUtils.hpp"
#include "shared/stringUtils.hpp"

#include <cstring>
#include <fstream>
#include <iostream>

// todo: decline invalid paths or something ig
// todo: test this properly again to see if it behaves as expected.
// todo: maybe make structs to class
// todo: maybe remove signals
namespace config {

	bool Parser::m_readingFile = false;

	const std::string Parser::WHITESPACE = "\t\r\v\f ";

	parse_exception::parse_exception(const std::string& message)
		: std::runtime_error("Parsing failed: " + message)
		, m_message("Parsing failed: " + message) {}

	parse_exception::parse_exception(std::size_t line, const std::string& message)
		: std::runtime_error("Line " + shared::string::toString(line) + ": Parsing failed: " + message)
		, m_message("Line " + shared::string::toString(line) + ": Parsing failed: " + message) {}

	parse_exception::~parse_exception() throw() {}

	const std::string& parse_exception::getMessage() const { return m_message; }

	Parser::Parser()
		: m_data()
		, m_config()
		, m_depth(0)
		, m_lineIndex(1)
		, m_readPos(0) {}

	Parser::~Parser() {}

	const Config& Parser::getConfig() { return m_config; }

	// ------------------------  utility  ----------------------------------- //

	void Parser::skipWhitespace() {
		while (m_readPos < m_data.size()) {
			char c = m_data[m_readPos];
			if (c == '\n') {
				++m_lineIndex;
				++m_readPos;
			} else if (WHITESPACE.find(c) != std::string::npos) {
				++m_readPos;
			} else {
				break; // no more whitespace
			}
		}
	}

	void Parser::consume(std::size_t amount) {
		if (amount + m_readPos > m_data.size()) {
			throw std::runtime_error("Parser: Cannot consume more data than available");
		}
		m_readPos += amount;
	}

	static bool isValidPath(const std::string& path) {
		return !path.empty() && path.at(0) == '/' && path.size() <= PATH_MAX;
	}

	static bool hasSinglePathSeparator(const std::string& path) {
		return path.find_first_of('/', 1) == std::string::npos;
	}

	// ------------------------  tokens and stuff  -------------------------- //

	// CONSUMES IF MATCHED
	bool Parser::matchToken(const std::string& token) {
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
	std::string Parser::readToken() {
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
	std::string Parser::readValue() {
		std::string value = "";
		skipWhitespace();
		while (m_readPos < m_data.size() && shouldReadValue(m_data[m_readPos])) {
			value.push_back(m_data[m_readPos]);
			consume(1);
		}
		if (!matchToken(";")) {
			throw parse_exception(m_lineIndex, "Expected semicolon after value \"" + value + "\"");
		}
		return value;
	}

	Parser::CommandType Parser::matchDirective(const std::string& token, const std::map<std::string, CommandType>& expectedDirectives) {
		std::map<std::string, CommandType>::const_iterator matchedDirective = expectedDirectives.find(token);
		if (matchedDirective == expectedDirectives.end()) {
			return _D_NOT_VALID;
		}
		consume(token.size());
		return matchedDirective->second;
	}

	const std::map<std::string, Parser::CommandType>& Parser::locationDirectives() {
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

	const std::map<std::string, Parser::CommandType>& Parser::serverDirectives() {
		static std::map<std::string, CommandType> allowedDirectives;

		if (allowedDirectives.empty()) {
			allowedDirectives["listen"] = D_LISTEN;
			allowedDirectives["client_max_body_size"] = D_CLIENT_MAX_BODY_SIZE;
			allowedDirectives["max_header_value_length"] = D_MAX_HEADER_VALUE_SIZE;
			allowedDirectives["max_header_count"] = D_MAX_HEADER_COUNT;
			allowedDirectives["max_header_value_count"] = D_MAX_HEADER_VALUE_COUNT;
			allowedDirectives["max_header_name_length"] = D_MAX_HEADER_NAME_SIZE;
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

	void Parser::handleSigint(int signum) {
		if (signum == SIGINT) {
			m_readingFile = false;
		}
	}

	// todo: maybe the server should still start with no config? just fall back to default and log a warning
	bool Parser::parseFile(const std::string& fileName) {
		signal(SIGINT, handleSigint);
		signal(SIGQUIT, SIG_IGN);

		if (shared::file::isDirectory(fileName)) {
			LOG_FATAL("Could not parse file: regular file expected, directory received");
			return true;
		}
		if (!shared::file::exists(fileName)) {
			LOG_FATAL("Could not parse file: file doesn't exist");
			return true;
		}
		if (!shared::file::isReadable(fileName)) {
			LOG_FATAL("Could not parse file: file can't be read");
			return true;
		}
		if (!shared::file::isRegularFile(fileName)) {
			LOG_FATAL("Could not parse config-file: expected valid file");
			return true;
		}
		std::ifstream file(fileName.c_str());
		if (!file.is_open()) {
			LOG_FATAL("Could not open config file: " + fileName + ": " + std::strerror(errno));
			return true;
		}

		char c;
		m_readingFile = true;
		// cppcheck-suppress knownConditionTrueFalse
		while (m_readingFile && file >> std::noskipws >> c) {
			if (c == '#') {
				while (file >> std::noskipws >> c && c != '\n') {}
			}
			m_data.push_back(c);
		}

		try {
			parseFromData();
			processParsedData();
		} catch (const parse_exception& e) {
			LOG_FATAL(e.getMessage());
			return true;
		} catch (const std::exception& e) {
			LOG_FATAL(e.what());
			return true;
		}
		return false;
	}

	void Parser::parseFromData() throw(parse_exception) {
		while (skipWhitespace(), m_readPos < m_data.size()) {
			if (!matchToken("server")) {
				throw parse_exception(m_lineIndex, "Expected 'server' keyword");
			}
			expectServerBlock();
		}
	}

	void Parser::processParsedData() throw(parse_exception) {
		if (m_config.serverConfigs.empty()) {
			throw parse_exception("No servers configured");
		}

		std::size_t i = 0;

		for (std::vector<ServerConfig>::iterator server = m_config.serverConfigs.begin(); server != m_config.serverConfigs.end(); ++server) {
			++i;
			server->location.precalculatedAbsolutePath = server->dataDirectory + server->location.root;
			if (!isValidPath(server->location.precalculatedAbsolutePath)) {
				throw parse_exception("Server #" + shared::string::toString(i) + ": Root path is invalid");
			}
			// TODO: check whether that directory exists and is accessible?
			if (!shared::file::isDirectory(server->location.precalculatedAbsolutePath)) {
				throw parse_exception("Server #" + shared::string::toString(i) + ": root is not a directory");
			}
			if (!shared::file::isReadable(server->location.precalculatedAbsolutePath)) {
				throw parse_exception("Server #" + shared::string::toString(i) + ": root is not readable");
			}
			if (!shared::file::isWritable(server->location.precalculatedAbsolutePath)) {
				throw parse_exception("Server #" + shared::string::toString(i) + ": root is not writable");
			}
			assignAbsolutePaths(*server, server->location); // start with server & rootLocation
		}
	}

	void Parser::assignAbsolutePaths(ServerConfig& server, LocationConfig& parentLocation) throw(parse_exception) {
		for (std::vector<LocationConfig>::iterator location = parentLocation.locations.begin(); location != parentLocation.locations.end(); ++location) {
			if (location->hasOwnRoot()) {
				location->precalculatedAbsolutePath = server.location.precalculatedAbsolutePath + location->root;
			} else {
				location->precalculatedAbsolutePath = parentLocation.precalculatedAbsolutePath;
			}
			if (!isValidPath(location->precalculatedAbsolutePath)) {
				throw parse_exception("Location \"" + location->path + "\": Path is invalid");
			}
			assignAbsolutePaths(server, *location);
		}
	}

	// ------------------------  block parsing ------------------------------ //

	/**
	 * Possible ServerConfig directives are:
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
	void Parser::expectServerBlock() throw(parse_exception) {
		skipWhitespace();
		if (!matchToken("{")) {
			throw parse_exception(m_lineIndex, "Expected opening brace after Server");
		}
		m_depth++;

		ServerConfig thisServer;

		while (m_readPos < m_data.size()) {
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '{') {
				throw parse_exception(m_lineIndex, "Unexpected opening brace in Server block");
			}
			if (matchToken("}")) {
				m_depth--;
				thisServer.validate();
				m_config.serverConfigs.push_back(thisServer);
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
				processServerValue(value, type, thisServer);
			}
		}
		throw parse_exception(m_lineIndex, "Server block not closed with '}'");
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
	void Parser::expectLocationBlock(LocationConfig& parentLocation) throw(parse_exception) {
		std::string path = readToken();

		if (path.empty()) {
			throw parse_exception(m_lineIndex, "Location has no path");
		}
		if (!isValidPath(path) || !hasSinglePathSeparator(path) || path == "/") {
			throw parse_exception(m_lineIndex, "Path is invalid: '" + path + "'");
		}
		consume(path.length());
		skipWhitespace();
		if (!matchToken("{")) {
			throw parse_exception(m_lineIndex, "Expected '{' after location path");
		}
		m_depth++;

		LocationConfig thisLocation;

		thisLocation.path = path;
		thisLocation.pathAsTokens = shared::string::split(path, '/');
		while (m_readPos <= m_data.size()) {
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '{') {
				throw parse_exception(m_lineIndex, "Unexpected opening brace in location block");
			}
			if (matchToken("}")) {
				m_depth--;
				thisLocation.validate();
				// validate LocationConfig whether enough stuff is present
				// is anything mandatory for a location?
				parentLocation.locations.push_back(thisLocation);
				return;
			}
			std::string token = readToken();
			if (matchToken("location")) {
				expectLocationBlock(thisLocation);
			} else if (!token.empty()) {
				CommandType type = matchDirective(token, locationDirectives());
				if (type == _D_NOT_VALID) {
					throw parse_exception(m_lineIndex, "Unexpected directive in Location block: " + token);
				}
				std::string value = readValue();
				processLocationValue(value, type, thisLocation);
			}
		}
		throw parse_exception(m_lineIndex, "Location block not closed with '}");
	}

	void Parser::processServerValue(const std::string& value, CommandType type, ServerConfig& server) {
		static std::map<CommandType, ServerTokenParser> tokenParsers;

		if (type > _D_SERVER_TYPES) {
			return processLocationValue(value, type, server.location);
		}
		if (tokenParsers.empty()) {
			tokenParsers[D_LISTEN] = &config::Parser::parseListen;
			tokenParsers[D_CLIENT_MAX_BODY_SIZE] = &config::Parser::parseClientMaxBodySize;
			tokenParsers[D_MAX_HEADER_VALUE_SIZE] = &config::Parser::parseMaxHeaderValueLength;
			tokenParsers[D_MAX_HEADER_COUNT] = &config::Parser::parseMaxHeaderCount;
			tokenParsers[D_MAX_HEADER_VALUE_COUNT] = &config::Parser::parseMaxHeaderValueCount;
			tokenParsers[D_MAX_HEADER_NAME_SIZE] = &config::Parser::parseMaxHeaderNameLength;
			tokenParsers[D_DATA_DIR] = &config::Parser::parseDataDir;
			tokenParsers[D_SERVER_NAME] = &config::Parser::parseServerName;
		}

		(this->*(tokenParsers[type]))(value, server);
	}

	// cppcheck-suppress constParameter
	void Parser::processLocationValue(const std::string& value, CommandType type, LocationConfig& location) {
		static std::map<CommandType, LocationTokenParser> tokenParsers;

		if (tokenParsers.empty()) {
			tokenParsers[D_ROOT] = &config::Parser::parseRoot;
			tokenParsers[D_RETURN] = &config::Parser::parseReturn;
			tokenParsers[D_LIMIT_EXCEPT] = &config::Parser::parseLimitExcept;
			tokenParsers[D_UPLOAD_DIR] = &config::Parser::parseUploadDir;
			tokenParsers[D_INDEX] = &config::Parser::parseIndex;
			tokenParsers[D_AUTOINDEX] = &config::Parser::parseAutoindex;
			// tokenParsers[D_LOCATION] = &config::Parser::parseLocation;
		}

		(this->*(tokenParsers[type]))(value, location);
	}

	// ------------------------  server parsers  ---------------------------- //

	uint32_t Parser::parseIpAddress(const std::string& host) {
		uint32_t ip = 0;
		std::size_t start = 0;
		std::size_t part = 0;

		while (part < 4) {
			std::size_t pos = host.find('.', start);
			std::string segment;
			if (pos == std::string::npos) {
				segment = host.substr(start);
			} else {
				segment = host.substr(start, pos - start);
			}
			if (segment.empty()) {
				throw parse_exception(m_lineIndex, "Invalid IP address format: empty segment in \"" + host + "\"");
			}
			for (std::size_t i = 0; i < segment.size(); ++i) {
				if (!std::isdigit(static_cast<unsigned char>(segment[i]))) {
					throw parse_exception(m_lineIndex, "Invalid IP address format: unexpected character '" + segment.substr(i, 1) + "' in \"" + host + "\"");
				}
			}
			if (segment.size() > 3) {
				throw parse_exception(m_lineIndex, "Invalid IP address format: segment too long in \"" + host + "\"");
			}
			int octet = shared::string::toNum<int>(segment);
			if (octet < 0 || octet > 255) {
				throw parse_exception(m_lineIndex, "IP segment out of range in \"" + host + "\"");
			}
			ip = (ip << 8) | (octet & 0xFF);
			part++;
			if (pos == std::string::npos)
				break;
			start = pos + 1;
		}

		if (part != 4) {
			throw parse_exception(m_lineIndex, "Invalid IP address format: \"" + host + "\"");
		}
		return ip;
	}

	void Parser::parseListen(const std::string& value, ServerConfig& server) {
		std::string host;
		std::string port;

		if (value.find_first_of(':') == std::string::npos) {
			throw parse_exception(m_lineIndex, "Invalid value for listen: " + value);
		}
		host = value.substr(0, value.find_first_of(':'));
		port = value.substr(value.find_first_of(':') + 1);

		std::stringstream ss(port);
		if (!(ss >> server.port)) {
			throw parse_exception(m_lineIndex, "Invalid value for listen: " + value);
		}
		server.ipAddress = parseIpAddress(host);
		if (ss >> port && !port.empty()) {
			throw parse_exception(m_lineIndex, "Invalid value for listen: " + value);
		}
	}

	void Parser::parseClientMaxBodySize(const std::string& value, ServerConfig& server) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> server.maxBodySize)) {
			throw parse_exception(m_lineIndex, "Invalid value for client_max_body_size");
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in client_max_body_size: " + token);
		}
	}

	void Parser::parseMaxHeaderValueLength(const std::string& value, ServerConfig& server) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> server.maxHeaderValueLength)) {
			throw parse_exception(m_lineIndex, "Invalid value for max_header_value_length");
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in max_header_value_length: " + token);
		}
	}

	void Parser::parseMaxHeaderCount(const std::string& value, ServerConfig& server) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> server.maxHeaderCount)) {
			throw parse_exception(m_lineIndex, "Invalid value for max_header_count");
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in max_header_count: " + token);
		}
	}

	void Parser::parseMaxHeaderValueCount(const std::string& value, ServerConfig& server) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> server.maxHeaderValueCount)) {
			throw parse_exception(m_lineIndex, "Invalid value for max_header_value_count");
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in max_header_value_count: " + token);
		}
	}

	void Parser::parseMaxHeaderNameLength(const std::string& value, ServerConfig& server) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> server.maxHeaderNameLength)) {
			throw parse_exception(m_lineIndex, "Invalid value for max_header_name_length");
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in max_header_name_length: " + token);
		}
	}

	void Parser::parseDataDir(const std::string& value, ServerConfig& server) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for data_dir: " + value);
		}
		server.dataDirectory = value; // check whether this needs to be stripped of whitespace
									  // TODO: should we normalize these?
	}

	void Parser::parseServerName(const std::string& value, ServerConfig& server) {
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

	void Parser::parseRoot(const std::string& value, LocationConfig& location) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for root: " + value);
		}
		location.root = value;
		location.rootAsTokens = shared::string::split(value, '/'); // will be empty if root is just '/'
	}

	void Parser::parseReturn(const std::string& value, LocationConfig& location) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for return: " + value);
		}
		location.redirectUriAsTokens = shared::string::split(value, '/'); // ensure this won't throw
		location.redirectUri = value;
	}

	// if no limit_except is present, default initialization
	// for locations should permit all methods; GET POST and DELETE
	void Parser::parseLimitExcept(const std::string& value, LocationConfig& location) {
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

	void Parser::parseUploadDir(const std::string& value, LocationConfig& location) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for upload_dir: " + value);
		}
		location.uploadSubdirectory = value;									 // strip this of whitespace if that doesn't happen yet
		location.uploadSubdirectoryAsTokens = shared::string::split(value, '/'); // strip this of whitespace if that doesn't happen yet
	}

	void Parser::parseIndex(const std::string& value, LocationConfig& location) {
		std::vector<std::string> indexFiles;
		std::stringstream ss(value);
		std::string token;

		while (ss >> token) {
			indexFiles.push_back(token);
		}
		if (indexFiles.empty()) {
			throw parse_exception(m_lineIndex, "Expected value for index");
		}
		location.indexFile = indexFiles;
	}

	void Parser::parseAutoindex(const std::string& value, LocationConfig& location) {
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

} /* namespace config */
