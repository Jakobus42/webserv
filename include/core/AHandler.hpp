#pragma once

#include "http/ClientSocket.hpp"
#include "http/VirtualServer.hpp"

namespace core {

	enum HandlerState {
		PENDING,
		PROCESSING,
		COMPLETED,
		FAILED
	};

	struct HandlerContext {
			http::VirtualServer &vServer;
			http::Connection &conn;
			uint32_t events;

			HandlerContext(http::VirtualServer &vServer, http::Connection &conn)
				: vServer(vServer)
				, conn(conn)
				, events(EPOLLIN) {}
	};

	class AHandler {
		public:
			AHandler()
				: m_state(PENDING) {}

			virtual void handle(HandlerContext &ctx) = 0;

			virtual bool hasCompleted() const {
				return m_state == COMPLETED;
			}

			virtual HandlerState getState() const {
				return m_state;
			}

			virtual ~AHandler() {}
		protected:
			HandlerState m_state;
	};

} // namespace core