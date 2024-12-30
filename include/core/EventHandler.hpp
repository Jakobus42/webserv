#pragma once
#include <sys/epoll.h>

#include "http/Request.hpp"
#include "http/Response.hpp"

#include <queue>

namespace http {
	class VirtualServer;
	class Connection;
} // namespace http

namespace core {

	typedef std::queue<http::Request> t_requests;
	typedef std::queue<http::Response> t_responses;

	enum HandlerState {
		PENDING_READ,
		PROCESSING,
		WAITING_FOR_WRITE,
		WRITING,
		SENDING,
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

			t_requests& getRequests(void);
			const t_requests& getRequests(void) const;
			t_responses& getResponses(void);
			const t_responses& getResponses(void) const;
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
			t_responses m_responses;
			http::Connection& m_connection;
			http::VirtualServer& m_server;
	};

} /* namespace core */
