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
			http::VirtualServer& vServer;
			http::Connection& conn;
			uint32_t events;

			HandlerContext(http::VirtualServer& vServer, http::Connection& conn)
				: vServer(vServer)
				, conn(conn)
				, events(EPOLLIN) {}

			HandlerContext& operator=(const HandlerContext& rhs) {
				if (this == &rhs)
					return *this;
				vServer = rhs.vServer;
				conn = rhs.conn;
				events = rhs.events;
				return *this;
			};
	};

	class AHandler {
		public:
			AHandler()
				: m_state(PENDING) {
				std::cout << "NEW HANDLER CREATED" << std::endl;
			}

			AHandler(const AHandler& other)
				: m_state(other.getState()) {}

			AHandler& operator=(const AHandler& rhs) {
				if (this == &rhs)
					return *this;
				m_state = rhs.getState();
				return *this;
			}

			virtual void handle(HandlerContext& ctx) = 0;
			virtual bool shouldDrop() const = 0;

			bool completed() const {
				return m_state == COMPLETED;
			}

			bool failed() const {
				return m_state == FAILED;
			}

			HandlerState getState() const {
				return m_state;
			}

			void setState(HandlerState state) {
				std::cout << "SETTING STATE TO " << state << std::endl;
				m_state = state;
			};

			virtual ~AHandler() {}

		protected:
			HandlerState m_state;
	};

} // namespace core