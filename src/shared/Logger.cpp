#include "shared/Logger.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include <ctime>
#include <iostream>

namespace shared {

	Logger::Logger()
		: m_level(DEBUG)
		, m_logFile() {
	}

	Logger::~Logger() {
		if (m_logFile.is_open()) {
			m_logFile.close();
		}
	}

	void Logger::log(const std::string& msg, LogLevel level) {
		if (level < m_level) {
			return;
		}

		std::ostream& stream = level > WARNING ? std::cout : std::cerr;
		const std::string formatted = "[" + getTimestamp() + "] " + "[" + levelToString(level) + "] " + msg;

		stream << formatted << std::endl;
		if (m_logFile.is_open()) {
			m_logFile << formatted << std::endl;
		}
	};

	Logger& Logger::getInstance() {
		static Logger instance;
		return instance;
	}

	void Logger::setLevel(LogLevel level) { m_level = level; }

	void Logger::setFile(const std::string& filename) {
		if (m_logFile.is_open()) {
			m_logFile.close();
		}

		const std::string dir = "logs/";
		if (mkdir(dir.c_str(), 0777) == -1 && errno != EEXIST) {
			throw std::runtime_error("Logger: can't create log directory");
		}

		m_logFile.open((dir + filename).c_str(), std::ios::app);
		if (!m_logFile.is_open()) {
			throw std::runtime_error("Logger: can't open new log file");
		}
	}

	LogLevel Logger::getLevel() const { return m_level; }

	const std::string& Logger::levelToString(LogLevel level) const {
		static const std::string levels[] = {
			"DEBUG",
			"TRACE",
			"INFO",
			"WARNING",
			"ERROR",
			"FATAL",
		};
		return levels[level];
	}

	std::string Logger::getTimestamp() const {
		char buffer[20];
		time_t now = time(NULL);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
		return std::string(buffer);
	}

} /* namespace shared */
