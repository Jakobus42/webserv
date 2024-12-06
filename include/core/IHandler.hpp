#pragma once

#include "http/ClientSocket.hpp"
#include "http/VirtualServer.hpp"

namespace core {

	struct HandleContext {
			http::VirtualServer &vServer;
			http::Connection &conn;

			HandleContext(http::VirtualServer &vServer, http::Connection &conn)
				: vServer(vServer)
				, conn(conn) {}
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
