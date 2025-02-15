#pragma once

#include "config/Server.hpp"

#include <map>
#include <vector>

namespace config {

	class parse_exception : public std::runtime_error {
		public:
			explicit parse_exception(std::size_t line);
			explicit parse_exception(const std::string& message);
			explicit parse_exception(std::size_t line, const std::string& message);
			virtual ~parse_exception() throw();

			const std::string& getMessage() const;

		private:
			std::string m_message;
	};

	enum CommandType {
		_D_NOT_VALID,
		D_LISTEN,
		D_CLIENT_MAX_BODY_SIZE,
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

	/**
	 * @class GoodParser
	 * @brief Parses the config goodly
	 */
	class GoodParser {
			typedef void (GoodParser::*LocationTokenParser)(const std::string&, Location&);
			typedef void (GoodParser::*ServerTokenParser)(const std::string&, Server&);

		public:
			GoodParser();
			~GoodParser();
			bool parseFile(const std::string& fileName);

		private:
			std::string m_data;
			std::vector<Server> m_configs;
			std::size_t m_depth;
			std::size_t m_lineIndex;
			std::size_t m_readPos;

			static const std::string WHITESPACE;

		private:
			GoodParser(const GoodParser& other);
			GoodParser& operator=(const GoodParser& rhs);

			void consume(std::size_t amount);
			bool matchToken(const std::string& token);
			std::string readToken();
			std::string readValue();
			void skipWhitespace();
			CommandType matchDirective(const std::string& token, const std::map<std::string, CommandType>& expectedDirectives);

			void parseFromData() throw(config::parse_exception);
			void parseLine(const std::string& line) throw(config::parse_exception);

			void expectServerBlock() throw(parse_exception);
			void expectLocationBlock(Location& parentLocation) throw(parse_exception);

			void setServerValue(const std::string& value, CommandType type, Server& server);
			void setLocationValue(const std::string& value, CommandType type, Location& location);

			static const std::map<std::string, CommandType>& locationDirectives();
			static const std::map<std::string, CommandType>& serverDirectives();

		private:
			// ------------------------  server parsers  -------------------- //
			void parsePort(const std::string& value, Server& server);
			void parseListen(const std::string& value, Server& server);
			void parseClientMaxBodySize(const std::string& value, Server& server);
			void parseDataDir(const std::string& value, Server& server);
			void parseServerName(const std::string& value, Server& server);

			// ------------------------  location parsers  ------------------ //
			void parseRoot(const std::string& value, Location& location);
			void parseReturn(const std::string& value, Location& location);
			void parseLimitExcept(const std::string& value, Location& location);
			void parseUploadDir(const std::string& value, Location& location);
			void parseIndex(const std::string& value, Location& location);
			void parseAutoindex(const std::string& value, Location& location);
			// void parseLocation(const std::string& value, Location& location);
	};
} /* namespace config */
