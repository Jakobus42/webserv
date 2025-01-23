#pragma once

#include "core/AHandler.hpp"
#include "core/Dispatcher.hpp"
#include "core/VirtualServer.hpp"

namespace core {

	/**
	 * @class AcceptHandler
	 * @brief ...
	 */
	class AcceptHandler : public AHandler {
		public:
			AcceptHandler(http::VirtualServer& vServer, Dispatcher& dispatcher);

			~AcceptHandler() {}

			void handle(int32_t fd, uint32_t events);

		private:
			http::VirtualServer& m_vServer;
			Dispatcher& m_dispatcher;
	};

} /* namespace core */
