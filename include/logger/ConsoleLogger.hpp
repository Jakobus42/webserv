#pragma once

#include "../../include/logger/ALogger.hpp"

namespace logger {

/**
 * @class ConsoleLogger
 * @brief Logs messages to the console.
 *
 * The `ConsoleLogger` class is a concrete implementation of the `ALogger`
 * abstract class, designed to output log messages to the standard output
 * (console). It supports different log levels and formats log messages
 * accordingly.
 *
 * This logger is suitable for scenarios where you want to see log messages in
 * the console for debugging or monitoring purposes. It inherits from `ALogger`,
 * which provides the foundational logging functionality.
 */
class ConsoleLogger : public ALogger {
 public:
  ConsoleLogger();
  virtual ~ConsoleLogger();
  ConsoleLogger(const ConsoleLogger& other);
  ConsoleLogger& operator=(const ConsoleLogger& other);

  void log(const std::string& message, LogLevel level);
};

} /* namespace logger */
