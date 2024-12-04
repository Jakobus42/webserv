#pragma once

#include <stdint.h>

#include "runtime/EventHandler.hpp"

namespace runtime {

/**
 * @class RequestHandler
 * @brief ...
 */
class RequestHandler : public EventHandler {
 public:
  RequestHandler();
  ~RequestHandler();

  void handle(int fd, uint32_t events);

 private:
  RequestHandler(const RequestHandler& other);
  RequestHandler& operator=(const RequestHandler& rhs);
};

} /* namespace runtime */
