#pragma once

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

typedef struct epoll_event t_event;

namespace http {

const uint32_t LOCALHOST_ADDRESS = 0x7F000001;   // 127.0.0.1
const uint32_t DEFAULT_MAX_BODY_SIZE = 1048576;  // 1MB

}  // namespace http
