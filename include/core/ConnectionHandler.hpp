#pragma once

#include <unistd.h>

#include "core/RequestProcessor.hpp"
#include "http/RequestParser.hpp"
#include "io/Dispatcher.hpp"
#include "io/IEventHandler.hpp"

#include <queue>

namespace http {
	class Request;
	class Response;
}; // namespace http

namespace core {

	class Connection;
	class VirtualServer;

	class ConnectionHandler : public io::IEventHandler {
		public:
			ConnectionHandler(VirtualServer* vServer, Connection* conn);
			virtual ~ConnectionHandler();

			virtual io::EventResult onReadable(int32_t fd);

			virtual io::EventResult onWriteable(int32_t fd);

			virtual io::EventResult onError(int32_t fd);

		private:
			io::EventResult unregister();

		private:
			VirtualServer* m_vServer;
			Connection* m_connection;

			http::RequestParser m_requestParser;
			RequestProcessor m_requestProcessor;

			std::size_t m_totalBytesSent;

			std::queue<http::Request*> m_requests;
			std::queue<http::Response*> m_responses;
	};

} /* namespace core */
