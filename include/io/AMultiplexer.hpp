#pragma once

#include <stdint.h>

#include "shared/NonCopyable.hpp"

#include <map>
#include <vector>

namespace io {

	class AMultiplexer : shared::mixin::NonCopyable {
		public:
			static const int32_t TIMEOUT_INFINITE = -1;
			static const int32_t MAX_EVENTS = 64;

			enum EventType {
				EVENT_READ = 0x1,
				EVENT_WRITE = 0x2,
				EVENT_ERROR = 0x4,
			};

			struct Event {
					int32_t fd;
					uint32_t events;

					Event(int32_t fd, uint32_t events)
						: fd(fd)
						, events(events) {}
			};

			typedef std::vector<Event> Events;

			AMultiplexer()
				: m_readyEvents()
				, m_registeredEvents() {}

			virtual ~AMultiplexer() {}

			virtual void add(int32_t fd, uint32_t events) = 0;
			virtual void modify(int32_t fd, uint32_t events) = 0;
			virtual void remove(int32_t fd) = 0;

			virtual int32_t poll(int32_t timeoutMs = TIMEOUT_INFINITE) = 0;

			const Events& getReadyEvents() const { return m_readyEvents; }

		protected:
			typedef std::map<int32_t, uint32_t> EventMap;

			Events m_readyEvents;
			EventMap m_registeredEvents;
	};

} /* namespace io */