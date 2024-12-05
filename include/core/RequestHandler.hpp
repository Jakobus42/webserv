#pragma once

#include <stdint.h>

namespace core {

/**
 * @class RequestHandler
 * @brief ...
 */
class RequestHandler {
 public:
  RequestHandler();
  ~RequestHandler();

  void handle(int fd, uint32_t events);

 private:
  RequestHandler(const RequestHandler& other);
  RequestHandler& operator=(const RequestHandler& rhs);
};

} /* namespace core */
