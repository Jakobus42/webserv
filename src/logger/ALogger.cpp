#include "../../include/logger/ALogger.hpp"

namespace logger {

/**
 * @brief Constructs a new ALogger object with the default log level set to
 * INFO.
 *
 * Initializes the `ALogger` object with a default log level of DEBUG.
 */
ALogger::ALogger() : m_currentLogLevel(logger::DEBUG) {}

/**
 * @brief Destroys the ALogger object.
 *
 * Cleans up resources used by the `ALogger` object.
 * This is a virtual destructor to ensure proper cleanup in derived classes.
 */
ALogger::~ALogger() {}

/**
 * @brief Copy constructor.
 *
 * Creates a new `ALogger` object as a copy of an existing one.
 *
 * @param other The `ALogger` object to copy from.
 */
ALogger::ALogger(const ALogger& other) : m_currentLogLevel(other.m_currentLogLevel) {}

/**
 * @brief Copy assignment operator.
 *
 * Assigns the state of another `ALogger` object to this one.
 *
 * @param other The `ALogger` object to assign from.
 * @return A reference to this `ALogger` object.
 */
ALogger& ALogger::operator=(const ALogger& other) {
  if (this != &other) {
    m_currentLogLevel = other.m_currentLogLevel;
  }
  return *this;
}

/**
 * @brief Sets the current log level.
 *
 * Updates the log level of the logger. Only messages with a level less than or
 * equal to this new level will be logged.
 *
 * @param level The new log level to set.
 */
void ALogger::setLevel(LogLevel level) { m_currentLogLevel = level; }

/**
 * @brief Returns the current log level.
 *
 * Retrieves the current log level of the logger.
 *
 * @return The current log level.
 */
LogLevel ALogger::getLevel() const { return m_currentLogLevel; }

/**
 * @brief Converts a log level to its string representation.
 *
 * Converts the specified log level to a readable string, such as "[INFO]" or
 * "[ERROR]".
 *
 * @param level The log level to convert.
 * @return A string representing the provided log level.
 */
std::string ALogger::getLogLevelAsString(LogLevel level) throw(std::invalid_argument) {
  switch (level) {
    case logger::DEBUG:
      return std::string(DEBUG_COLOR) + "[DEBUG]  " + RESET;
    case logger::INFO:
      return std::string(INFO_COLOR) + "[INFO]   " + RESET;
    case logger::WARNING:
      return std::string(WARNING_COLOR) + "[WARNING]" + RESET;
    case logger::ERROR:
      return std::string(ERROR_COLOR) + "[ERROR]  " + RESET;
    case logger::FATAL:
      return std::string(FATAL_COLOR) + "[FATAL]  " + RESET;
  }
  throw std::invalid_argument("invalid log level");
}

/**
 * @brief Retrieves the current timestamp in a formatted string.
 *
 * This method obtains the current local time and formats it as a string in the
 * format "YYYY-MM-DD HH:MM:SS". The timestamp is enclosed in square brackets.
 *
 * Example output: "[2024-09-11 14:30:45]"
 *
 * @return std::string A string representing the current timestamp.
 * @throws std::runtime_error If the conversion of the time to local time fails.
 */
std::string ALogger::getTimeStamp() throw(std::runtime_error) {
  char buffer[20];
  time_t now = time(0);
  tm* now_tm = localtime(&now);
  if (!now_tm) {
    throw std::runtime_error("[ERROR] failed to convert time to local time.");
  }

  strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", now_tm);
  return '[' + std::string(buffer) + ']';
}

/**
 * @brief Creates Prefix for Logs.
 *
 * This method calls getLogLevelAsString and getTimeStamp to create the log msg
 * Prefix. Example output: "[DEBUG]   [2024-09-11 14:30:45]"
 *
 * @return std::string representing the prefix for the log msg.
 * @throws std::runtime_error If the conversion of the time to local time fails.
 */
std::string ALogger::getLogPrefix(LogLevel level) throw(std::runtime_error) {
  return getLogLevelAsString(level) + getTimeStamp();
}

} /* namespace logger */
