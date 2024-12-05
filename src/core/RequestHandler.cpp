#include "core/RequestHandler.hpp"

namespace core {

/**
 * @brief Constructs a new RequestHandler object.
 */
RequestHandler::RequestHandler() {}

/**
 * @brief Destroys the RequestHandler object.
 */
RequestHandler::~RequestHandler() {}

/**
 * @brief Copy constructor.
 * @param other The other RequestHandler object to copy.
 */
RequestHandler::RequestHandler(const RequestHandler& other) { (void)other; }

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

} /* namespace core */
