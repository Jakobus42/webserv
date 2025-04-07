#pragma once

#include "io/Multiplexer.hpp"

namespace io {

	class IEventHandler;

	class Dispatcher : shared::mixin::NonCopyable {
		public:
			Dispatcher();
			~Dispatcher();

			void registerHandler(int32_t fd, IEventHandler* handler, uint32_t events = Multiplexer::EVENT_ERROR | Multiplexer::EVENT_READ | Multiplexer::EVENT_WRITE);
			void modifyEvents(int32_t fd, uint32_t events);
			void unregisterHandler(int32_t fd);

			int32_t dispatch(int32_t timeoutMs = AMultiplexer::TIMEOUT_INFINITE);

		private:
			typedef std::map<int32_t, IEventHandler*> HandlerMap;

			Multiplexer m_multiplexer;
			HandlerMap m_handlers;

			void handleEvent(const AMultiplexer::Event& event);
			void secureUnregisterHandler(int32_t fd);
	};

} /* namespace io */