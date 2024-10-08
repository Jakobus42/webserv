#pragma once

#include <map>
#include <memory>

#include "../../include/logger/ALogger.hpp"
#include "../../include/logger/ConsoleLogger.hpp"
#include "../../include/logger/FileLogger.hpp"

namespace logger {

/**
 * @class LoggerManager
 * @brief Manages multiple loggers and handles global logging operations.
 *
 * The `LoggerManager` class is a singleton that manages a collection of
 * loggers, each identified by a unique name. It provides functionality to
 * register and unregister loggers, set log levels, and log messages to all
 * registered loggers. It also supports setting and retrieving a global log
 * level that controls the logging behavior across all loggers.
 */
class LoggerManager {
 public:
  static LoggerManager& getInstance();

  void registerLogger(const std::string& loggerName, ALogger* logger);
  void unregisterLogger(const std::string& loggerName);
  void log(const std::string& message, LogLevel level);

  void setLoggerLevel(const std::string& loggerName, LogLevel level);
  void setGlobalLogLevel(LogLevel level);
  LogLevel getGlobalLogLevel() const;

 private:
  typedef std::map<std::string, ALogger*> LogMap;

  LoggerManager();
  ~LoggerManager(){};
  LoggerManager(const LoggerManager&) : m_globalLogLevel(DEBUG){};
  LoggerManager& operator=(const LoggerManager& other);

  LogMap m_loggers;
  LogLevel m_globalLogLevel;
};

// https://gcc.gnu.org/onlinedocs/gcc-3.1.1/gcc/Function-Names.html#:~:text=GCC%20predefines%20two%20magic%20identifiers,in%20a%20language%20specific%20fashion.
/**
 * @brief Logs a message using the LoggerManager.
 *
 * This macro logs a message to the logger with the current file, line number,
 * and function name. It provides a detailed log entry including the source
 * location and the message itself.
 *
 * @param message The message to log.
 * @param level The log level for the message (e.g., DEBUG, INFO, WARNING,
 * ERROR, FATAL).
 *
 * Usage:
 * @code
 * LOG("This is a test message", logger::INFO);
 * @endcode
 */
#define LOG(message, level)                                         \
  std::ostringstream oss;                                           \
  oss << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": " \
      << message;                                                   \
  logger::LoggerManager::getInstance().log(oss.str(), level);
} /* namespace logger */