#include "runtime/RequestHandler.hpp"

namespace runtime {

/**
 * @brief Constructs a new RequestHandler object.
 */
RequestHandler::RequestHandler() : EventHandler() {}

/**
 * @brief Destroys the RequestHandler object.
 */
RequestHandler::~RequestHandler() {}

/**
 * @brief Copy constructor.
 * @param other The other RequestHandler object to copy.
 */
RequestHandler::RequestHandler(const RequestHandler& other) : EventHandler() { (void)other; }

/**
 * @brief Copy assignment operator.
 * @param other The other RequestHandler object to assign from.
 * @return A reference to the assigned RequestHandler object.
 */
RequestHandler& RequestHandler::operator=(const RequestHandler& rhs) {
  (void)rhs;
  return *this;
}

void RequestHandler::handle(int fd, uint32_t event) {
  (void)fd;
  (void)event;
}

} /* namespace runtime */
