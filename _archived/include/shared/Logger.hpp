#pragma once

#include "shared/NonCopyable.hpp"

#include <fstream>

namespace shared {

	enum LogLevel {
		DEBUG,
		TRACE,
		INFO,
		WARNING,
		ERROR,
		FATAL,
	};

	/**
	 * @class Logger
	 * @brief ...
	 */
	class Logger : NonCopyable {
		public:
			Logger();
			Logger(const std::string& filename);
			~Logger();

			void log(const std::string& msg, LogLevel level = DEBUG);

			static Logger& getInstance();

			void setLevel(LogLevel level);
			void setFile(const std::string& filename);
			LogLevel getLevel() const;

		private:
			const std::string& levelToString(LogLevel level) const;
			std::string getTimestamp() const;

		private:
			static const std::string RESET;
			static const std::string DEBUG_COLOR;
			static const std::string TRACE_COLOR;
			static const std::string INFO_COLOR;
			static const std::string WARNING_COLOR;
			static const std::string ERROR_COLOR;
			static const std::string FATAL_COLOR;

			LogLevel m_level;
			std::ofstream m_logFile;
	};

}; /* namespace shared */

#define LOG(msg, level) shared::Logger::getInstance().log(msg, level);
