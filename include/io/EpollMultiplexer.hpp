#pragma once

#include "io/AMultiplexer.hpp"

namespace io {

	class EpollMultiplexer : public AMultiplexer {
		public:
			EpollMultiplexer();
			virtual ~EpollMultiplexer();

			virtual void add(int32_t fd, uint32_t events);
			virtual void modify(int32_t fd, uint32_t events);
			virtual void remove(int32_t fd);

			virtual int32_t poll(int32_t timeoutMs = TIMEOUT_INFINITE);

		private:
			uint32_t convertToEpollEvents(uint32_t events) const;
			uint32_t convertFromEpollEvents(uint32_t epollEvents) const;
	};

} /* namespace io */