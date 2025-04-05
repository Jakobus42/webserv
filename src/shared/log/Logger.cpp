#include "shared/log/Logger.hpp"

#include <ctime>
#include <iostream>
#include <sstream>

namespace shared {
	namespace log {

		Logger::ScopedContext::ScopedContext(const std::string& ctx) { Logger::instance().pushContext(ctx); }

		Logger::ScopedContext::~ScopedContext() { Logger::instance().popContext(); }

		const char* const Logger::LEVEL_STRINGS[] = {
			"DEBUG",
			"INFO",
			"WARNING",
			"ERROR",
			"FATAL"};

		Logger& Logger::instance() {
			static Logger instance;
			return instance;
		}

		Logger::Logger()
			: m_logFile()
			, m_minLevel(DEBUG)
			, m_contexts()
			, m_consoleOutputEnabled(true) {}

		Logger::~Logger() {
			if (m_logFile.is_open()) {
				m_logFile.close();
			}
		}

		void Logger::debug(const std::string& message) { log(DEBUG, message); }

		void Logger::info(const std::string& message) { log(INFO, message); }

		void Logger::warning(const std::string& message) { log(WARNING, message); }

		void Logger::error(const std::string& message) { log(ERROR, message); }

		void Logger::fatal(const std::string& message) { log(FATAL, message); }

		void Logger::log(LogLevel level, const std::string& message) {
			if (level < m_minLevel)
				return;

			std::ostringstream formattedMessage;
			formattedMessage << "[" << getCurrentTimestamp() << "] ";
			formattedMessage << "[" << LEVEL_STRINGS[level] << "] ";
			const std::string contextStr = formatContext();
			if (!contextStr.empty()) {
				formattedMessage << "[" << contextStr << "] ";
			}
			formattedMessage << message << std::endl;

			if (m_consoleOutputEnabled) {
				std::ostream& os = (level >= WARNING) ? std::cerr : std::cout;
				os << formattedMessage.str();
			}
			if (m_logFile.is_open()) {
				m_logFile << formattedMessage.str() << std::endl;
				m_logFile.flush();
			}
		}

		void Logger::setLogLevel(LogLevel level) { m_minLevel = level; }

		void Logger::setLogFile(const std::string& path) {
			if (m_logFile.is_open()) {
				m_logFile.close();
			}
			m_logFile.open(path.c_str(), std::ios::app);
		}

		void Logger::setConsoleOutputEnabled(bool enabled) { m_consoleOutputEnabled = enabled; }

		void Logger::pushContext(const std::string& ctx) { m_contexts.push_back(ctx); }

		void Logger::popContext() {
			if (!m_contexts.empty()) {
				m_contexts.pop_back();
			}
		}

		std::string Logger::formatContext() const {
			std::string contextStr;
			for (std::vector<std::string>::const_iterator it = m_contexts.begin(); it != m_contexts.end(); ++it) {
				if (it != m_contexts.begin()) {
					contextStr += " > ";
				}
				contextStr += *it;
			}
			return contextStr;
		}

		std::string Logger::getCurrentTimestamp() const {
			std::time_t now = std::time(0);
			struct std::tm* timeinfo = std::localtime(&now);

			char buffer[80];
			std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
			return std::string(buffer);
		}

	} // namespace log
} // namespace shared