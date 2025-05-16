#pragma once

#include "config/Config.hpp"
#include "config/ParseException.hpp"
#include "config/ServerConfig.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/stringUtils.hpp"

#include <map>

namespace config {

	class Parser : shared::mixin::NonCopyable {
			typedef void (Parser::*LocationTokenParser)(const std::string&, LocationConfig&);
			typedef void (Parser::*ServerTokenParser)(const std::string&, ServerConfig&);
			typedef void (Parser::*HttpTokenParser)(const std::string&, HttpConfig&);

		public:
			Parser();
			~Parser();

			bool parseFile(const std::string& fileName);
			const Config& getConfig();

		private:
			std::string m_data;
			Config m_config;
			std::size_t m_depth;
			std::size_t m_lineIndex;
			std::size_t m_readPos;
			static bool m_readingFile;

			std::map<std::string, http::Method> m_implementedMethods;
			std::set<std::string> m_autoIndexAllowedValues;

			static const std::string WHITESPACE;

		private:
			enum CommandType {
				_D_NOT_VALID,
				D_CLIENT_MAX_BODY_SIZE,
				D_MAX_HEADER_VALUE_SIZE,
				D_MAX_HEADER_COUNT,
				D_MAX_HEADER_VALUE_COUNT,
				D_MAX_HEADER_NAME_SIZE,
				D_MAX_URI_LENGTH,
				D_CGI_TIMEOUT,
				D_CGI_INTERPRETER,
				D_CONNECTION_TIMEOUT,
				_D_HTTP_TYPES,
				D_LISTEN,
				D_SERVER_NAME,
				_D_SERVER_TYPES,
				D_ROOT,
				D_RETURN,
				D_LIMIT_EXCEPT,
				D_UPLOAD_DIR,
				D_INDEX,
				D_AUTOINDEX,
				D_ERROR_PAGE,
				D_LOCATION
			};

			void consume(std::size_t amount);
			bool matchToken(const std::string& token);
			std::string readToken();
			std::string readValue();
			void skipWhitespace();
			CommandType matchDirective(const std::string& token, const std::map<std::string, CommandType>& expectedDirectives);
			static void handleSigint(int signum);

			void parseFromData() throw(ParseException);
			void processParsedData() throw(ParseException);

			void expectHttpBlock() throw(ParseException);
			void expectServerBlock(const HttpConfig& globalConfig) throw(ParseException);
			void expectLocationBlock(LocationConfig& parentLocation) throw(ParseException);

			void processHttpValue(const std::string& key, const std::string& value, CommandType type) throw(ParseException);
			void processServerValue(const std::string& key, const std::string& value, CommandType type, ServerConfig& server) throw(ParseException);
			void processLocationValue(const std::string& key, const std::string& value, CommandType type, LocationConfig& location) throw(ParseException);

			static const std::map<std::string, CommandType>& httpDirectives();
			static const std::map<std::string, CommandType>& serverDirectives();
			static const std::map<std::string, CommandType>& locationDirectives();

		private:
			// ------------------------  generic parsers  ------------------- //
			void parseIntegerValue(const std::string& key, const std::string& value, unsigned long& destination) throw(ParseException);
			void parsePathValue(const std::string& key, const std::string& value, std::string& destination) throw(ParseException);
			// ------------------------  server parsers  -------------------- //
			void parseListen(const std::string& value, ServerConfig& server) throw(ParseException);
			void parseServerName(const std::string& value, ServerConfig& server) throw(ParseException);
			void parseCGIInterpreter(const std::string& value, HttpConfig& globalConfig) throw(ParseException);

			uint32_t parseIpAddress(const std::string& host) throw(ParseException);
			void assignPaths(ServerConfig& server, LocationConfig& parentLocation) throw(ParseException);

			// ------------------------  location parsers  ------------------ //
			void parseReturn(const std::string& value, LocationConfig& location) throw(ParseException);
			void parseLimitExcept(const std::string& value, LocationConfig& location) throw(ParseException);
			void parseIndex(const std::string& value, LocationConfig& location) throw(ParseException);
			void parseAutoindex(const std::string& value, LocationConfig& location) throw(ParseException);
			void parseErrorPage(const std::string& value, LocationConfig& location) throw(ParseException);
	};

} /* namespace config */
