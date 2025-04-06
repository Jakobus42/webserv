#pragma once

#include <stdint.h>

#include "shared/NonCopyable.hpp"

#include <cstddef>
#include <map>

namespace io {

	enum Event {
		EVENT_READ = 0x1,
		EVENT_WRITE = 0x2,
		EVENT_ERROR = 0x4,
	};

	class AHandler;

	class AMultiplexer : shared::mixin::NonCopyable {
		public:
			static const int32_t TIMEOUT_INFINITE = -1;

			AMultiplexer()
				: m_handlers() {}

			virtual ~AMultiplexer() {}

			virtual void subscribe(int32_t fd, uint32_t events, AHandler* handler) = 0;
			virtual void modify(int32_t fd, uint32_t events) = 0;
			virtual void unsubscribe(int32_t fd) = 0;

			virtual int32_t dispatch(int32_t timeoutMs = TIMEOUT_INFINITE) = 0;

		protected:
			struct HandlerEntry {
					AHandler* handler;
					uint32_t events;

					HandlerEntry()
						: handler(NULL)
						, events(0) {}

					HandlerEntry(AHandler* handler, uint32_t events)
						: handler(handler)
						, events(events) {}
			};

			typedef std::map<int32_t /*fd*/, HandlerEntry> HandlerMap;

			HandlerMap m_handlers;
	};

} /* namespace io */
