#pragma once

#include "core/Dispatcher.hpp"
#include "core/IHandler.hpp"
#include "core/VirtualServer.hpp"

namespace core {

	/**
	 * @class AcceptHandler
	 * @brief ...
	 */
	class AcceptHandler : public IHandler {
		public:
			AcceptHandler(http::VirtualServer& vServer, Dispatcher& dispatcher);

			~AcceptHandler() {}

			void handle(int32_t fd, uint32_t events);

		private:
			http::VirtualServer& m_vServer;
			Dispatcher& m_dispatcher;
	};

} /* namespace core */
