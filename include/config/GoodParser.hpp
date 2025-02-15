#pragma once

#include "config/Server.hpp"

#include <vector>

namespace config {

	class parse_exception : public std::runtime_error {
		public:
			explicit parse_exception(std::size_t line);
			explicit parse_exception(std::size_t line, const std::string& message);
			virtual ~parse_exception() throw();

			const std::string& getMessage() const;

		private:
			std::string m_message;
	};

	/**
	 * @class GoodParser
	 * @brief ...
	 */
	class GoodParser {
		public:
			GoodParser();
			~GoodParser();
			bool parseFile(const std::string& fileName);

		private:
			std::string m_data;
			std::vector<Server> m_configs;
			std::size_t m_depth;
			std::size_t m_lineIndex;
			std::size_t m_writePos;

		private:
			GoodParser(const GoodParser& other);
			GoodParser& operator=(const GoodParser& rhs);

			void consume(std::size_t amount);
			bool matchToken(const std::string& token);
			std::string readToken();
			void skipWhitespace();

			void parseFromData() throw(config::parse_exception);
			void parseLine(const std::string& line) throw(config::parse_exception);

			void expectServerBlock() throw(parse_exception);
			void expectLocationBlock(Location& parentLocation) throw(parse_exception);
	};
} /* namespace config */
