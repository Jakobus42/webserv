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
		, m_readPos(0)
		, m_implementedMethods()
		, m_autoIndexAllowedValues() {
		m_implementedMethods["GET"] = http::GET;
		m_implementedMethods["POST"] = http::POST;
		m_implementedMethods["DELETE"] = http::DELETE;
		m_autoIndexAllowedValues.insert("on");
		m_autoIndexAllowedValues.insert("off");
	}

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
			allowedDirectives["error_page"] = D_ERROR_PAGE;
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
			allowedDirectives["data_dir"] = D_DATA_DIR;
			allowedDirectives["server_name"] = D_SERVER_NAME;
			allowedDirectives["root"] = D_ROOT;
			allowedDirectives["return"] = D_RETURN;
			allowedDirectives["limit_except"] = D_LIMIT_EXCEPT;
			allowedDirectives["upload_dir"] = D_UPLOAD_DIR;
			allowedDirectives["index"] = D_INDEX;
			allowedDirectives["autoindex"] = D_AUTOINDEX;
			allowedDirectives["location"] = D_LOCATION;
			allowedDirectives["error_page"] = D_ERROR_PAGE;
			allowedDirectives["connection_timeout"] = D_CONNECTION_TIMEOUT;
			allowedDirectives["cgi_timeout"] = D_CGI_TIMEOUT;
			allowedDirectives["cgi_interpreter"] = D_CGI_INTERPRETER;
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

		for (std::vector<ServerConfig>::const_iterator it = m_config.serverConfigs.begin(); it != m_config.serverConfigs.end(); ++it) {
			m_config.listenServerConfigs[it->socketAddress].push_back(*it);
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
			throw parse_exception(m_lineIndex, "Expected '{' after 'server'");
		}
		m_depth++;

		ServerConfig thisServer;

		thisServer.location.path = "/";
		thisServer.location.isServerRoot = true;

		while (m_readPos < m_data.size()) {
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '{') {
				throw parse_exception(m_lineIndex, "Unexpected '{' in Server block");
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
				processServerValue(token, value, type, thisServer);
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
			throw parse_exception(m_lineIndex, "Location path is invalid: '" + path + "'");
		}
		consume(path.length());
		skipWhitespace();
		if (!matchToken("{")) {
			throw parse_exception(m_lineIndex, "Expected '{' after location path");
		}
		m_depth++;

		LocationConfig thisLocation;

		thisLocation.path = path;
		while (m_readPos <= m_data.size()) {
			skipWhitespace();
			if (m_readPos < m_data.size() && m_data[m_readPos] == '{') {
				throw parse_exception(m_lineIndex, "Unexpected '{' in location block");
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
				processLocationValue(token, value, type, thisLocation);
			}
		}
		throw parse_exception(m_lineIndex, "Location block not closed with '}");
	}

	void Parser::processServerValue(const std::string& key, const std::string& value, CommandType type, ServerConfig& server) throw(parse_exception) {
		static std::map<CommandType, ServerTokenParser> tokenParsers;

		if (type > _D_SERVER_TYPES) {
			return processLocationValue(key, value, type, server.location);
		}
		if (tokenParsers.empty()) {
			tokenParsers[D_LISTEN] = &config::Parser::parseListen;
			tokenParsers[D_SERVER_NAME] = &config::Parser::parseServerName;
			tokenParsers[D_CGI_INTERPRETER] = &config::Parser::parseCGIInterpreter;
		}

		switch (type) {
			case D_CLIENT_MAX_BODY_SIZE:
				return parseIntegerValue("client_max_body_size", value, server.maxBodySize);
			case D_MAX_HEADER_COUNT:
				return parseIntegerValue("max_header_count", value, server.maxHeaderCount);
			case D_MAX_HEADER_VALUE_COUNT:
				return parseIntegerValue("max_header_value_count", value, server.maxHeaderValueCount);
			case D_MAX_HEADER_NAME_SIZE:
				return parseIntegerValue("max_header_name_length", value, server.maxHeaderNameLength);
			case D_MAX_HEADER_VALUE_SIZE:
				return parseIntegerValue("max_header_value_length", value, server.maxHeaderValueLength);
			case D_CONNECTION_TIMEOUT:
				return parseIntegerValue("connection_timeout", value, server.connectionTimeout);
			case D_CGI_TIMEOUT:
				return parseIntegerValue("cgi_timeout", value, server.cgiTimeout);
			case D_DATA_DIR:
				return parsePathValue("data_dir", value, server.dataDirectory);
			default:
				return (this->*(tokenParsers[type]))(value, server);
		}
	}

	void Parser::processLocationValue(const std::string& key, const std::string& value, CommandType type, LocationConfig& location) throw(parse_exception) {
		static std::map<CommandType, LocationTokenParser> tokenParsers;

		if (tokenParsers.empty()) {
			tokenParsers[D_RETURN] = &config::Parser::parseReturn;
			tokenParsers[D_LIMIT_EXCEPT] = &config::Parser::parseLimitExcept;
			tokenParsers[D_INDEX] = &config::Parser::parseIndex;
			tokenParsers[D_AUTOINDEX] = &config::Parser::parseAutoindex;
			tokenParsers[D_ERROR_PAGE] = &config::Parser::parseErrorPage;
		}

		switch (type) {
			case D_ROOT:
				return parsePathValue(key, value, location.root);
			case D_UPLOAD_DIR:
				return parsePathValue(key, value, location.uploadSubdirectory);
			default:
				return (this->*(tokenParsers[type]))(value, location);
		}
	}

	// ------------------------  server parsers  ---------------------------- //

	void Parser::parseIntegerValue(const std::string& key, const std::string& value, unsigned long& destination) throw(parse_exception) {
		std::stringstream ss(value);
		std::string token;

		if (!(ss >> destination)) {
			throw parse_exception(m_lineIndex, "Invalid value for " + key);
		}
		if (ss >> token) {
			throw parse_exception(m_lineIndex, "Unexpected token in " + key + ": " + token);
		}
	}

	void Parser::parsePathValue(const std::string& key, const std::string& value, std::string& destination) throw(parse_exception) {
		if (!isValidPath(value)) {
			throw parse_exception(m_lineIndex, "Invalid path for " + key + ": " + value);
		}
		destination = value; // strip this of whitespace if that doesn't happen yet
	}

	uint32_t Parser::parseIpAddress(const std::string& host) throw(parse_exception) {
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

	void Parser::parseListen(const std::string& value, ServerConfig& server) throw(parse_exception) {
		std::string host;
		std::string port;

		if (value.find_first_of(':') == std::string::npos) {
			throw parse_exception(m_lineIndex, "Invalid value for listen: " + value);
		}
		host = value.substr(0, value.find_first_of(':'));
		port = value.substr(value.find_first_of(':') + 1);

		server.socketAddress = host + ":" + port;
		std::stringstream ss(port);
		if (!(ss >> server.port)) {
			throw parse_exception(m_lineIndex, "Invalid value for listen: " + value);
		}
		server.ipAddress = parseIpAddress(host);
		if (ss >> port && !port.empty()) {
			throw parse_exception(m_lineIndex, "Invalid value for listen: " + value);
		}
	}

	void Parser::parseServerName(const std::string& value, ServerConfig& server) throw(parse_exception) {
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

	void Parser::parseCGIInterpreter(const std::string& value, ServerConfig& server) throw(parse_exception) {
		std::vector<std::string> args;
		std::stringstream ss(value);
		std::string token;

		while (ss >> token) {
			args.push_back(token);
		}
		if (args.size() != 2)
			throw parse_exception(m_lineIndex, "Expected 2 arguments for cgi_interpreter");
		if (shared::file::isExecutable(args[1]) == false) {
			throw parse_exception(m_lineIndex, "Path does not lead to an executable: " + args[1]);
		}
		// if already exists in interpreters, throw exception
		server.cgiInterpreters[args[0]] = args[1];
	}

	// ------------------------  location parsers  -------------------------- //

	void Parser::parseReturn(const std::string& value, LocationConfig& location) throw(parse_exception) {
		std::vector<std::string> args;
		std::stringstream ss(value);
		std::string token;
		std::size_t num_buffer;

		while (ss >> token) {
			args.push_back(token);
		}
		if (args.size() != 2) {
			throw parse_exception(m_lineIndex, "Expected 2 arguments for return");
		}
		std::stringstream ss_num(args[0]);
		if (!(ss_num >> num_buffer)) {
			throw parse_exception(m_lineIndex, "Invalid redirect code: " + args[0]);
		}
		http::StatusCode status_code = http::numToStatusCode(num_buffer);
		if (status_code < 300 || status_code >= 400) {
			throw parse_exception(m_lineIndex, "Invalid redirect code: " + args[0]);
		}
		if (!isValidPath(args[1])) {
			throw parse_exception(m_lineIndex, "Invalid path for return: " + args[1]);
		}

		location.redirectUri = std::make_pair(status_code, args[1]);
	}

	// if no limit_except is present, default initialization
	// for locations should permit all methods; GET POST and DELETE
	void Parser::parseLimitExcept(const std::string& value, LocationConfig& location) throw(parse_exception) {
		std::set<http::Method> allowedMethods;
		std::stringstream ss(value);
		std::string token;

		while (ss >> token) {
			if (m_implementedMethods.find(token) == m_implementedMethods.end()) {
				throw parse_exception(m_lineIndex, "Unexpected method for limit_except: " + token);
			}
			if (allowedMethods.find(m_implementedMethods[token]) != allowedMethods.end()) {
				throw parse_exception(m_lineIndex, "Duplicte method for limit_except: " + token);
			}
			allowedMethods.insert(m_implementedMethods[token]);
		}
		if (allowedMethods.empty()) {
			throw parse_exception(m_lineIndex, "Expected value for limit_except");
		}
		location.allowedMethods = allowedMethods;
	}

	void Parser::parseIndex(const std::string& value, LocationConfig& location) throw(parse_exception) {
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

	void Parser::parseAutoindex(const std::string& value, LocationConfig& location) throw(parse_exception) {
		std::stringstream ss(value);
		std::string token;

		ss >> token;
		if (m_autoIndexAllowedValues.find(token) == m_autoIndexAllowedValues.end()) {
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

	void Parser::parseErrorPage(const std::string& value, LocationConfig& location) throw(parse_exception) {
		std::vector<std::string> args;
		std::stringstream ss(value);
		std::string token;
		int num_buffer;

		while (ss >> token) {
			args.push_back(token);
		}
		if (args.size() < 2) {
			throw parse_exception(m_lineIndex, "Expected at least 2 arguments for error_page");
		}
		std::string errorPagePath = args.back();
		for (unsigned long i = 0; i < args.size() - 1; i++) {
			std::stringstream ss_num(args[i]);
			if (!(ss_num >> num_buffer)) {
				throw parse_exception(m_lineIndex, "Invalid error code: " + args[i]);
			}
			http::StatusCode status_code;
			try {
				status_code = http::numToStatusCode(num_buffer);
			}
			catch(const std::exception &e) {
				throw parse_exception(m_lineIndex, "Invalid error code: " + args[0] + ": " + e.what());
			}
			if (status_code < 400) {
				throw parse_exception(m_lineIndex, "Invalid error code: " + args[0]);
			}
			location.errorPages[status_code] = errorPagePath;
		}
	}

} /* namespace config */
