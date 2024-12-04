#include "runtime/EventHandler.hpp"

namespace runtime {

/**
 * @brief Constructs a new EventHandler object.
 */
EventHandler::EventHandler() {}

/**
 * @brief Destroys the EventHandler object.
 */
EventHandler::~EventHandler() {}

/**
 * @brief Copy constructor.
 * @param other The other EventHandler object to copy.
 */
EventHandler::EventHandler(const EventHandler& other) { (void)other; }

/**
 * @brief Copy assignment operator.
 * @param other The other EventHandler object to assign from.
 * @return A reference to the assigned EventHandler object.
 */
EventHandler& EventHandler::operator=(const EventHandler& rhs) {
  (void)rhs;
  return *this;
}

} /* namespace runtime */
