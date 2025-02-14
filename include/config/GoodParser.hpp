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
			std::vector<Server> m_configs;
			std::size_t m_depth;

		private:
			GoodParser(const GoodParser& other);
			GoodParser& operator=(const GoodParser& rhs);

			void parseFromString(const std::string& fileContent) throw(config::parse_exception);
			void parseLine(const std::string& line, std::size_t lineIndex) throw(config::parse_exception);
	};

} /* namespace config */
