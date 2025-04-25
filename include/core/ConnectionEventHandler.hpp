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

	class ConnectionEventHandler : public io::IEventHandler {
		public:
			ConnectionEventHandler(VirtualServer* vServer, Connection* conn);
			virtual ~ConnectionEventHandler();

			virtual io::EventResult onReadable(int32_t fd);

			virtual io::EventResult onWriteable(int32_t fd);

			virtual io::EventResult onError(int32_t fd);

		private:
			VirtualServer* m_vServer;
			Connection* m_connection;

			http::RequestParser m_requestParser;
			RequestProcessor m_requestProcessor;

			std::size_t m_totalBytesSent; // todo: maybe find a better way to not have this in here

			std::queue<http::Request*> m_requests;
			std::queue<http::Response*> m_responses;
	};

} /* namespace core */
