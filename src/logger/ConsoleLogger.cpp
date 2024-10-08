#include "../../include/logger/ConsoleLogger.hpp"

namespace logger {

/**
 * @brief Constructs a new ConsoleLogger object.
 *
 * Initializes a `ConsoleLogger` object. This constructor calls the base class
 * constructor to initialize the logging level.
 */
ConsoleLogger::ConsoleLogger() : ALogger() {}

/**
 * @brief Destroys the ConsoleLogger object.
 *
 * Cleans up resources used by the `ConsoleLogger` object. This destructor is
 * empty because `ConsoleLogger` does not own any dynamically allocated
 * resources.
 */
ConsoleLogger::~ConsoleLogger() {}

/**
 * @brief Copy constructor.
 *
 * Creates a new `ConsoleLogger` object as a copy of an existing one. This copy
 * constructor initializes the new object by calling the base class constructor.
 *
 * @param other The `ConsoleLogger` object to copy from.
 */
ConsoleLogger::ConsoleLogger(const ConsoleLogger&) : ALogger() {}

/**
 * @brief Copy assignment operator.
 *
 * Assigns the state of another `ConsoleLogger` object to this one. This
 * operator allows copying of `ConsoleLogger` objects, but as `ConsoleLogger`
 * does not have additional data members beyond those inherited from `ALogger`,
 * the assignment is trivial.
 *
 * @param other The `ConsoleLogger` object to assign from.
 * @return A reference to this `ConsoleLogger` object.
 */
ConsoleLogger& ConsoleLogger::operator=(const ConsoleLogger& other) {
  ALogger::operator=(other);
  return *this;
}

/**
 * @brief Logs a message to the console.
 *
 * Outputs a log message to the console if the current log level allows it.
 * The message is prefixed with the log level converted to a string
 * representation.
 *
 * @param message The message to log.
 * @param level The log level of the message.
 */
void ConsoleLogger::log(const std::string& message, LogLevel level) {
  if (m_currentLogLevel <= level) {
    try {
      std::cout << getLogPrefix(level) << ' ' << message << std::endl;
    } catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

} /* namespace logger */
