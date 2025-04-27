#pragma once

#if defined(__APPLE__)
#	define PLATFORM_HAS_KQUEUE 1
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#	define PLATFORM_HAS_KQUEUE 1
#elif defined(__linux__)
#	define PLATFORM_HAS_EPOLL 1
#else
#	error "unsupported platform"
#endif // platform check

#if defined(SUBMISSION_BUILD)
#	define ALLOW_FORBIDDEN_FUNCTIONS 0
#else
#	define ALLOW_FORBIDDEN_FUNCTIONS 1
#endif // submission check