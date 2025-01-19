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
			LogLevel getLevel() const;

		private:
			const std::string& levelToString(LogLevel level) const;
			std::string getTimestamp() const;

		private:
			LogLevel m_level;
			std::ofstream m_logFile;
	};

#define LOG(msg, level) shared::Logger::getInstance().log(msg, level);

} /* namespace shared */
