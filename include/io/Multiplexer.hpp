#pragma once

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#	include "io/KqueueMultiplexer.hpp"

namespace io {
	typedef KqueueMultiplexer Multiplexer;
}

#elif defined(__linux__)
#	include "io/EpollMultiplexer.hpp"

namespace io {
	typedef EpollMultiplexer Multiplexer;
}

#else
#	error "Unsupported platform"
#endif