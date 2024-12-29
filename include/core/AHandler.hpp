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
			EPOLL_EVENTS events;

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
			AHandler(const HandlerContext& ctx)
				: m_state(PENDING)
				, m_context(ctx) {
				std::cout << "NEW HANDLER CREATED" << std::endl;
			}

			AHandler(const AHandler& other)
				: m_state(other.getState())
				, m_context(other.getContext()) {}

			AHandler& operator=(const AHandler& rhs) {
				if (this == &rhs)
					return *this;
				m_state = rhs.getState();
				m_context = rhs.getContext();
				return *this;
			}

			virtual void handle() = 0;
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

			const HandlerContext& getContext() const {
				return m_context;
			}

			HandlerContext& getContext() {
				return m_context;
			}

			void setState(HandlerState state) {
				std::cout << "SETTING STATE TO " << state << std::endl;
				m_state = state;
			};

			void setContext(HandlerContext context) {
				std::cout << "SETTING CONTEXT TO ANOTHER WITH EVENT " << context.events << std::endl;
				m_context = context;
			}

			virtual ~AHandler() {}

		protected:
			HandlerState m_state;
			HandlerContext m_context;

		private:
			AHandler();
	};

} // namespace core
