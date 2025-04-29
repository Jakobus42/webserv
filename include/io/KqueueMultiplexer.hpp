#pragma once

#include "io/AMultiplexer.hpp"

namespace io {

	class KqueueMultiplexer : public AMultiplexer {
		public:
			KqueueMultiplexer();
			virtual ~KqueueMultiplexer();

			virtual void add(int32_t fd, uint32_t events);
			virtual void modify(int32_t fd, uint32_t events);
			virtual void remove(int32_t fd);

			virtual int32_t poll(int32_t timeoutMs = TIMEOUT_INFINITE);

		private:
			uint32_t convertFromKqueueEvents(int16_t filter, uint16_t flags) const;
	};

} /* namespace io */