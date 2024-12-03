#include "logger/LoggerManager.hpp"

namespace logger {

/**
 * @brief Constructs a new LoggerManager object.
 *
 * Initializes the `LoggerManager` object with a default global log level of
 * `INFO`.
 */
LoggerManager::LoggerManager() : m_globalLogLevel(logger::INFO) {}

/**
 * @brief Destructor for the LoggerManager class.
 *
 * This destructor is responsible for cleaning up all logger objects stored in
 * the `m_loggers` map. It iterates over each logger and deletes the dynamically
 * allocated logger instances to avoid memory leaks.
 */
LoggerManager::~LoggerManager() {
  for (LogMap::iterator it = m_loggers.begin(); it != m_loggers.end(); ++it) {
    delete it->second;
  }
}

/**
 * @brief Copy assignment operator.
 *
 * Assigns the state of another `LoggerManager` object to this one. This
 * operator allows copying of `LoggerManager` objects, but as `LoggerManager`
 * does not have additional data members beyond those inherited from `ALogger`,
 * the assignment is trivial.
 *
 * @param other The `LoggerManager` object to assign from.
 * @return A reference to this `LoggerManager` object.
 */
LoggerManager& LoggerManager::operator=(const LoggerManager& other) {
  if (this != &other) {
    m_globalLogLevel = other.m_globalLogLevel;
  }
  return *this;
};

/**
 * @brief Provides access to the singleton instance of LoggerManager.
 *
 * This method ensures that only one instance of `LoggerManager` exists and
 * provides access to it. The instance is created on first use and is returned
 * for all subsequent calls.
 *
 * @return A reference to the singleton `LoggerManager` instance.
 */
LoggerManager& LoggerManager::getInstance() {
  static LoggerManager instance;
  return instance;
}

/**
 * @brief Registers a new logger with the manager.
 *
 * Adds a logger to the manager's collection, associating it with a specified
 * name. If a logger with the same name already exists, it will be replaced.
 *
 * @param loggerName The name to associate with the logger.
 * @param logger A shared pointer to the `ALogger` object to register.
 */
void LoggerManager::registerLogger(const std::string& loggerName, ALogger* logger) {
  m_loggers.insert(std::make_pair(loggerName, logger));
}

/**
 * @brief Unregisters a logger from the manager.
 *
 * Removes the logger associated with the given name from the manager's
 * collection. If no logger with the specified name exists, this operation has
 * no effect.
 *
 * @param loggerName The name of the logger to unregister.
 */
void LoggerManager::unregisterLogger(const std::string& loggerName) {
  LogMap::iterator it = m_loggers.find(loggerName);
  if (it != m_loggers.end()) {
    delete it->second;
    m_loggers.erase(it);
  }
}

/**
 * @brief Logs a message to all registered loggers.
 *
 * Sends a log message to all loggers that have been registered with the
 * manager, provided that the global log level allows for the message to be
 * logged.
 *
 * @param message The message to log.
 * @param level The log level of the message.
 */
void LoggerManager::log(const std::string& message, LogLevel level) {
  for (LogMap::iterator it = m_loggers.begin(); it != m_loggers.end(); ++it) {
    if (m_globalLogLevel <= level) {
      it->second->log(message, level);
    }
  }
}

/**
 * @brief Sets the log level for a specific logger.
 *
 * Changes the logging level of a specific logger by its name. Only messages
 * with a log level equal to or higher than the new level will be logged by that
 * logger.
 *
 * @param loggerName The name of the logger whose level to set.
 * @param level The new log level for the specified logger.
 */
void LoggerManager::setLoggerLevel(const std::string& loggerName, LogLevel level) {
  LogMap::iterator it = m_loggers.find(loggerName);
  if (it != m_loggers.end()) {
    it->second->setLevel(level);
  }
}

/**
 * @brief Sets the global log level.
 *
 * Changes the global log level. Only messages with a log level equal to or
 * higher than the new global level will be processed by the manager.
 *
 * @param level The new global log level.
 */
void LoggerManager::setGlobalLogLevel(LogLevel level) { m_globalLogLevel = level; }

/**
 * @brief Gets the current global log level.
 *
 * Retrieves the global log level, which determines the minimum level of
 * messages that will be processed by the manager.
 *
 * @return The current global log level.
 */
LogLevel LoggerManager::getGlobalLogLevel() const { return m_globalLogLevel; }

} /* namespace logger */
