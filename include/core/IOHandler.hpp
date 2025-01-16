#pragma once

#include "core/Dispatcher.hpp"
#include "core/IHandler.hpp"
#include "http/VirtualServer.hpp"

// todo: maybe have dispatcher as member to handle stay alive etc.
namespace core {

	/**
	 * @class IOHandler
	 * @brief ...
	 */
	class IOHandler : public IHandler {
		public:
			IOHandler(http::VirtualServer& vServer, http::Connection& conn);
			~IOHandler();

			void handle(int32_t fd, uint32_t events);

		private:
			http::VirtualServer& m_vServer;
			http::Connection& m_connection;

			// todo: add a map for method to handler
	};

} /* namespace core */
