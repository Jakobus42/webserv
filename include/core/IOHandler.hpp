#pragma once

#include "core/Dispatcher.hpp"
#include "core/IHandler.hpp"
#include "core/VirtualServer.hpp"

// todo: maybe have dispatcher as member to handle stay alive etc.
namespace core {

	/**
	 * @class IOHandler
	 * @brief ...
	 */
	class IOHandler : public IHandler {
		public:
			IOHandler(http::VirtualServer& vServer);
			~IOHandler();

			void handle(int32_t fd, uint32_t events);

		private:
			void handleRead(int32_t fd);
			void handleWrite(int32_t fd);

		private:
			http::VirtualServer& m_vServer;
	};

} /* namespace core */
