#include "core/AcceptEventHandler.hpp"

#include "core/ConnectionEventHandler.hpp"
#include "shared/Logger.hpp"

namespace core {

	AcceptEventHandler::AcceptEventHandler(core::VirtualServer& vServer, io::Dispatcher& dispatcher)
		: m_vServer(vServer)
		, m_dispatcher(dispatcher) {}

	AcceptEventHandler::~AcceptEventHandler() {}

	io::EventResult AcceptEventHandler::onReadable(int32_t) {
		LOG_CONTEXT("read event | server: " + m_vServer.getVirtualServerInfo());

		while (Connection* conn = m_vServer.acceptNewConnection()) {
			m_dispatcher.registerHandler(conn->getSocket().getFd(),
										 new ConnectionEventHandler(m_vServer, *conn),
										 io::AMultiplexer::EVENT_ALL);
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult AcceptEventHandler::onWriteable(int32_t) {
		LOG_CONTEXT("write event | server: " + m_vServer.getVirtualServerInfo());
		m_vServer.shutdown();
		return io::UNREGISTER;
	}

	io::EventResult AcceptEventHandler::onError(int32_t) {
		LOG_CONTEXT("error event | server: " + m_vServer.getVirtualServerInfo());
		m_vServer.shutdown();
		return io::UNREGISTER;
	}

} /* namespace core */
