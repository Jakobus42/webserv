#pragma once

#include <stdint.h>
#include <sys/epoll.h>

#include "core/IHandler.hpp"

#include <map>

namespace core {

	/**
	 * @class Dispatcher
	 * @brief ...
	 */
	class Dispatcher : shared::NonCopyable {
		public:
			Dispatcher();
			~Dispatcher();

			void registerHandler(int32_t fd, uint32_t events, IHandler* handler);
			void unregisterHandler(int32_t fd);

			void dispatch();

		private:
			static const std::size_t MAX_EVENTS = 256;

			int32_t m_epoll_master_fd;
			epoll_event m_events[MAX_EVENTS];

			std::map<int32_t, IHandler*> m_handlers;
	};

} /* namespace core */
