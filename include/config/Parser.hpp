#pragma once

#include "config/ServerConfig.hpp"
#include "shared/NonCopyable.hpp"

#include <map>

namespace config {

	class parse_exception : public std::runtime_error {
		public:
			explicit parse_exception(const std::string& message);
			explicit parse_exception(std::size_t line, const std::string& message);
			virtual ~parse_exception() throw();

			const std::string& getMessage() const;

		private:
			std::string m_message;
	};

	struct Config {
			std::vector<ServerConfig> serverConfigs;
	};

	class Parser : shared::mixin::NonCopyable {
			typedef void (Parser::*LocationTokenParser)(const std::string&, LocationConfig&);
			typedef void (Parser::*ServerTokenParser)(const std::string&, ServerConfig&);

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
				D_LISTEN,
				D_CLIENT_MAX_BODY_SIZE,
				D_MAX_HEADER_VALUE_SIZE,
				D_MAX_HEADER_COUNT,
				D_MAX_HEADER_VALUE_COUNT,
				D_MAX_HEADER_NAME_SIZE,
				D_DATA_DIR,
				D_SERVER_NAME,
				_D_SERVER_TYPES,
				D_ROOT,
				D_RETURN,
				D_LIMIT_EXCEPT,
				D_UPLOAD_DIR,
				D_INDEX,
				D_AUTOINDEX,
				D_LOCATION
			};

			void consume(std::size_t amount);
			bool matchToken(const std::string& token);
			std::string readToken();
			std::string readValue();
			void skipWhitespace();
			CommandType matchDirective(const std::string& token, const std::map<std::string, CommandType>& expectedDirectives);
			static void handleSigint(int signum);

			void parseFromData() throw(parse_exception);
			void processParsedData() throw(parse_exception);

			void expectServerBlock() throw(parse_exception);
			void expectLocationBlock(LocationConfig& parentLocation) throw(parse_exception);

			void processServerValue(const std::string& key, const std::string& value, CommandType type, ServerConfig& server) throw(parse_exception);
			void processLocationValue(const std::string& key, const std::string& value, CommandType type, LocationConfig& location) throw(parse_exception);

			static const std::map<std::string, CommandType>& locationDirectives();
			static const std::map<std::string, CommandType>& serverDirectives();

		private:
			// ------------------------  generic parsers  ------------------- //
			void parseIntegerValue(const std::string& key, const std::string& value, unsigned long& destination) throw(parse_exception);
			void parsePathValue(const std::string& key, const std::string& value, std::string& destination) throw(parse_exception);
			// ------------------------  server parsers  -------------------- //
			void parseListen(const std::string& value, ServerConfig& server) throw(parse_exception);
			void parseServerName(const std::string& value, ServerConfig& server) throw(parse_exception);

			uint32_t parseIpAddress(const std::string& host) throw(parse_exception);
			void assignAbsolutePaths(ServerConfig& server, LocationConfig& parentLocation) throw(parse_exception);

			// ------------------------  location parsers  ------------------ //
			void parseReturn(const std::string& value, LocationConfig& location) throw(parse_exception);
			void parseLimitExcept(const std::string& value, LocationConfig& location) throw(parse_exception);
			void parseIndex(const std::string& value, LocationConfig& location) throw(parse_exception);
			void parseAutoindex(const std::string& value, LocationConfig& location) throw(parse_exception);
	};

} /* namespace config */
