#pragma once

#include "io/Dispatcher.hpp"
#include "io/IEventHandler.hpp"

namespace core {

	class Connection;
	class VirtualServer;

	class ConnectionHandler : public io::IEventHandler {
		public:
			ConnectionHandler(VirtualServer* vServer, Connection* conn, io::Dispatcher& dispatcher); // ig this also need vServer?
			virtual ~ConnectionHandler();

			virtual io::EventResult onReadable(int32_t fd);

			virtual io::EventResult onWriteable(int32_t fd);

			virtual io::EventResult onError(int32_t fd);

		private:
			VirtualServer* m_vServer;
			Connection* m_connection;
			io::Dispatcher& m_dispatcher;
	};

} /* namespace core */
