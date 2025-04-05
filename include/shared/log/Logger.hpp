#pragma once

#include <fstream>
#include <shared/mixin/NonCopyable.hpp>
#include <vector>

namespace shared {
	namespace log {

		enum LogLevel {
			DEBUG,
			INFO,
			WARNING,
			ERROR,
			FATAL
		};

		class Logger {
			public:
				class ScopedContext {
					public:
						ScopedContext(const std::string& ctx);
						~ScopedContext();
				};

				static Logger& instance();

				void log(LogLevel level, const std::string& message);

				void debug(const std::string& message);
				void info(const std::string& message);
				void warning(const std::string& message);
				void error(const std::string& message);
				void fatal(const std::string& message);

				void setLogLevel(LogLevel level);
				void setLogFile(const std::string& path);
				void setConsoleOutputEnabled(bool enabled);

				void pushContext(const std::string& ctx);
				void popContext();

			private:
				Logger();
				~Logger();

				std::string formatContext() const;
				std::string getCurrentTimestamp() const;

			private:
				static const char* const LEVEL_STRINGS[];

				std::ofstream m_logFile;
				LogLevel m_minLevel;
				std::vector<std::string> m_contexts;
				bool m_consoleOutputEnabled;
		};

	} // namespace log
} // namespace shared

#define LOG_DEBUG(msg) shared::log::Logger::instance().debug(msg)
#define LOG_INFO(msg) shared::log::Logger::instance().info(msg)
#define LOG_WARNING(msg) shared::log::Logger::instance().warning(msg)
#define LOG_ERROR(msg) shared::log::Logger::instance().error(msg)
#define LOG_FATAL(msg) shared::log::Logger::instance().fatal(msg)
#define LOG_CONTEXT(ctx) shared::log::Logger::ScopedContext __logCtx##__LINE__(ctx)
