#pragma once

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

namespace logger {

const std::string RESET = "\033[0m";
const std::string DEBUG_COLOR = "\033[90m";    // Dark Gray
const std::string INFO_COLOR = "\033[34m";     // Dark Blue
const std::string WARNING_COLOR = "\033[33m";  // Yellow
const std::string ERROR_COLOR = "\033[31m";    // Red
const std::string FATAL_COLOR = "\033[35m";    // Magenta

enum LogLevel { DEBUG, INFO, WARNING, ERROR, FATAL };

/**
 * @class ALogger
 * @brief Abstract base class for logging functionality.
 *
 * `ALogger` provides a framework for logging systems, managing log levels
 * (DEBUG, INFO, WARNING, ERROR, FATAL) and formatting log messages with
 * color-coding. It defines:
 *
 * - `setLevel()` and `getLevel()`: To manage log levels.
 * - `log()`: To log messages (to be implemented by derived classes).
 * - `getLogLevelAsString()`: Returns a string representation of the log level,
 * with colors.
 *
 * Derived classes must implement the logging logic.
 */

class ALogger {
 public:
  ALogger();
  virtual ~ALogger();
  ALogger(const ALogger& other);
  ALogger& operator=(const ALogger& other);

  void setLevel(LogLevel level);
  LogLevel getLevel() const;

  virtual void log(const std::string& message, LogLevel level) = 0;

 protected:
  LogLevel m_currentLogLevel;

  static std::string getLogPrefix(LogLevel level) throw(std::runtime_error);

 private:
  static std::string getLogLevelAsString(LogLevel level) throw(std::invalid_argument);
  static std::string getTimeStamp() throw(std::runtime_error);
};

} /* namespace logger */
