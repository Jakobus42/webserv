#include "../../include/logger/FileLogger.hpp"

namespace logger {

/**
 * @brief Constructs a new FileLogger object with a default log file name.
 *
 * Initializes the `FileLogger` object with a default log file name "log".
 * If the file cannot be opened, an error message is printed.
 */
FileLogger::FileLogger() : ALogger(), m_logFile("log", std::ofstream::out) {
  if (!m_logFile.is_open()) {
    std::cerr << "Failed to open log file: " << std::endl;
  }
}

/**
 * @brief Constructs a new FileLogger object with a specified file name.
 *
 * @param filename The name of the file to log messages to.
 * Initializes the `FileLogger` object with the specified file name.
 * If the file cannot be opened, an error message is printed.
 */
FileLogger::FileLogger(const std::string& filename)
    : ALogger(), m_logFile(filename.c_str(), std::ofstream::out) {
  if (!m_logFile.is_open()) {
    std::cerr << "Failed to open log file: " << filename << std::endl;
  }
}

/**
 * @brief Destroys the FileLogger object.
 *
 * Closes the log file if it is open.
 */
FileLogger::~FileLogger() {
  if (m_logFile.is_open()) {
    m_logFile.close();
  }
}

/**
 * @brief Copy constructor.
 *
 * Initializes a new `FileLogger` object as a copy of an existing one.
 *
 * @param other The `FileLogger` object to copy.
 */
FileLogger::FileLogger(const FileLogger& other) : ALogger(other) {}

/**
 * @brief Copy assignment operator.
 *
 * Assigns the state of another `FileLogger` object to this one.
 *
 * @param other The `FileLogger` object to assign from.
 * @return A reference to this `FileLogger` object.
 */
FileLogger& FileLogger::operator=(const FileLogger& other) {
  if (this != &other) {
    ALogger::operator=(other);
  }
  return *this;
}

/**
 * @brief Logs a message to the file.
 *
 * @param message The message to log.
 * @param level The log level of the message.
 */
void FileLogger::log(const std::string& message, LogLevel level) {
  if (!m_logFile.is_open()) {
    std::cerr << "Log file is not open." << std::endl;
    return;
  }
  if (m_currentLogLevel <= level) {
    try {
      m_logFile << getLogPrefix(level) << ' ' << message << std::endl;
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

} /* namespace logger */
