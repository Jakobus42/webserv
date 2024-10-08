#pragma once

#include <fstream>

#include "../../include/logger/ALogger.hpp"

namespace logger {

/**
 * @class FileLogger
 * @brief Logs messages to a specified file.
 *
 * The `FileLogger` class is a concrete implementation of the `ALogger` abstract
 * class, designed to output log messages to a file. It supports different log
 * levels and formats log messages accordingly. This logger is suitable for
 * scenarios where you want to persist log messages in a file for later analysis
 * or record-keeping.
 */
class FileLogger : public ALogger {
 public:
  FileLogger();
  FileLogger(const std::string& filename);
  virtual ~FileLogger();
  FileLogger(const FileLogger& other);
  FileLogger& operator=(const FileLogger& other);

  void log(const std::string& message, LogLevel level);

 private:
  std::ofstream m_logFile;
};

} /* namespace logger */
