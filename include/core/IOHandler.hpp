#pragma once

#include "core/AHandler.hpp"
#include "core/Dispatcher.hpp"
#include "core/VirtualServer.hpp"
#include "http/RequestParser.hpp"
#include "http/RequestProccesor.hpp"
#include "http/Response.hpp"

#include <queue>

// todo: maybe have dispatcher as member to handle stay alive etc.
namespace core {

	/**
	 * @class IOHandler
	 * @brief ...
	 */
	class IOHandler : public AHandler {
		public:
			IOHandler(http::VirtualServer& vServer);
			~IOHandler();

			void handle(int32_t fd, uint32_t events);

		private:
			void handleRead(int32_t fd);
			void handleWrite(int32_t fd);

		private:
			http::VirtualServer& m_vServer;

			http::RequestParser m_reqParser;
			http::RequestProccesor m_reqProccesor;

			std::queue<http::Response*> m_responses; // todo que
			bool m_keepAlive;
	};

} /* namespace core */
