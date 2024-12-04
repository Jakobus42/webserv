#pragma once

#include <stdint.h>

namespace runtime {

/**
 * @class EventHandler
 * @brief ...
 */
class EventHandler {
 public:
  EventHandler();
  virtual ~EventHandler();

  virtual void handle(int fd, uint32_t events) = 0;

 private:
  EventHandler(const EventHandler& other);
  EventHandler& operator=(const EventHandler& rhs);
};

} /* namespace runtime */
