#pragma once

#include "shared/platform.hpp"

#if defined(PLATFORM_HAS_KQUEUE)
#	include "io/KqueueMultiplexer.hpp"

namespace io {
	typedef KqueueMultiplexer Multiplexer;
}

#elif defined(PLATFORM_HAS_EPOLL)
#	include "io/EpollMultiplexer.hpp"

namespace io {
	typedef EpollMultiplexer Multiplexer;
}

#else
#	error "unsupported platform: no multiplexer available"
#endif // platform check