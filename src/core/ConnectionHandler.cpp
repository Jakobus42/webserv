#include "core/ConnectionHandler.hpp"

namespace core {

	ConnectionHandler::ConnectionHandler(Connection* conn, io::Dispatcher& dispatcher)
		: m_connection(conn)
		, m_dispatcher(dispatcher) {
		(void)m_connection;
		(void)m_dispatcher;
	}

	ConnectionHandler::~ConnectionHandler() {
	}

	io::EventResult ConnectionHandler::onReadable(int32_t) {
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionHandler::onWriteable(int32_t) {
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionHandler::onError(int32_t) {
		return io::UNREGISTER;
	}

} /* namespace core */
