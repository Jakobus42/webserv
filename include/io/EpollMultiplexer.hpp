#pragma once

#if defined(__linux__)

#	include "io/AMultiplexer.hpp"

namespace io {

	class EpollMultiplexer : AMultiplexer {
		public:
			EpollMultiplexer();
			virtual ~EpollMultiplexer();

			virtual void subscribe(int32_t fd, uint32_t events, AHandler* handler);
			virtual void modify(int32_t fd, uint32_t events);
			virtual void unsubscribe(int32_t fd);

			virtual int32_t dispatch(int32_t timeoutMs = TIMEOUT_INFINITE);

		private:
			uint32_t convertToEpollEvents(uint32_t events) const;

		private:
			static const int16_t MAX_EVENTS = 256;

			int32_t m_epollFd;
	};

} /* namespace io */

#endif // platform check