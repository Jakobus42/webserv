#include "core/AcceptHandler.hpp"

#include "core/ConnectionHandler.hpp"
#include "shared/Logger.hpp"

namespace core {

	AcceptHandler::AcceptHandler(core::VirtualServer* vServer, io::Dispatcher& dispatcher)
		: m_vServer(vServer)
		, m_dispatcher(dispatcher) {}

	AcceptHandler::~AcceptHandler() {}

	io::EventResult AcceptHandler::onReadable(int32_t) {
		LOG_CONTEXT("read event | server: " + m_vServer->getVirtualServerInfo());

		while (Connection* conn = m_vServer->acceptNewConnection()) {
			m_dispatcher.registerHandler(conn->getSocket()->getFd(),
										 new ConnectionHandler(m_vServer, conn),
										 io::AMultiplexer::EVENT_ALL);
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult AcceptHandler::onWriteable(int32_t) {
		LOG_CONTEXT("write event | server: " + m_vServer->getVirtualServerInfo());
		m_vServer->shutdown();
		return io::UNREGISTER;
	}

	io::EventResult AcceptHandler::onError(int32_t) {
		LOG_CONTEXT("error event | server: " + m_vServer->getVirtualServerInfo());
		m_vServer->shutdown();
		return io::UNREGISTER;
	}

} /* namespace core */
