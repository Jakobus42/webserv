#pragma once

#include "http/ClientSocket.hpp"
#include "http/VirtualServer.hpp"

namespace core {

	struct HandleContext {
			http::VirtualServer &vServer;
			http::Connection &conn;
			uint32_t events;

			HandleContext(http::VirtualServer &vServer, http::Connection &conn, uint32_t events)
				: vServer(vServer)
				, conn(conn)
				, events(events) {}
	};

	/**
	 * @class IHandler
	 * @brief ...
	 */
	class IHandler {
		public:
			virtual void handle(HandleContext &ctx) = 0;

			virtual ~IHandler() {}
	};
} /* namespace core */
