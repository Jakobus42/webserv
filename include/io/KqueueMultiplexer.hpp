#pragma once

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#	include "io/AMultiplexer.hpp"

namespace io {

	class KqueueMultiplexer : AMultiplexer {
		public:
			KqueueMultiplexer();
			virtual ~KqueueMultiplexer();

			virtual void subscribe(int32_t fd, uint32_t events, AHandler* handler);
			virtual void modify(int32_t fd, uint32_t events);
			virtual void unsubscribe(int32_t fd);

			virtual int32_t dispatch(int32_t timeoutMs = TIMEOUT_INFINITE);

		private:
			static const int16_t MAX_EVENTS = 256;

			int32_t m_kqueueFd;
	};

} /* namespace io */

#endif // platform check
