#pragma once
#include <sys/epoll.h>

#include "http/Request.hpp"

#include <queue>

namespace http {
	class VirtualServer;
	class Connection;
} // namespace http

namespace core {

	typedef std::queue<http::Request> t_requests;

	enum HandlerState {
		PENDING,
		PROCESSING,
		COMPLETED,
		FAILED
	};

	/**
	 * @class EventHandler
	 * @brief ...
	 */
	class EventHandler {
		public:
			EventHandler(http::VirtualServer& server, http::Connection& connection, uint32_t events);
			~EventHandler();
			EventHandler(const EventHandler& other);
			EventHandler& operator=(const EventHandler& rhs);

			std::queue<http::Request>& getRequests(void);
			const std::queue<http::Request>& getRequests(void) const;
			HandlerState getState(void) const;
			http::VirtualServer& getServer(void);
			http::VirtualServer& getServer(void) const;
			http::Connection& getConnection(void);
			http::Connection& getConnection(void) const;
			uint32_t getEvents(void) const;

			void setState(HandlerState);

			bool completed(void) const;
			bool failed(void) const;
			bool shouldDrop(void) const;
			void killSelf(void);

			void handleRequest(void);
			void handleResponse(void);
			void handleError(void);

		private:
			EventHandler();

			uint32_t m_events;
			HandlerState m_state;
			t_requests m_requests;
			http::Connection& m_connection;
			http::VirtualServer& m_server;
	};

} /* namespace core */
