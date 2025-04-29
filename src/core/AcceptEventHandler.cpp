#include "core/AcceptEventHandler.hpp"

#include "core/ConnectionEventHandler.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

namespace core {

	AcceptEventHandler::AcceptEventHandler(core::VirtualServer& vServer, io::Dispatcher& dispatcher)
		: m_vServer(vServer)
		, m_dispatcher(dispatcher) {}

	AcceptEventHandler::~AcceptEventHandler() {
		const std::vector<Connection*>& connections = m_vServer.getActiveConnections();
		for (std::size_t i = 0; i < connections.size(); ++i) {
			int fd = connections[i]->getSocket().getFd();
			try {
				m_dispatcher.unregisterHandler(fd);
			} catch (const std::exception& e) {
				LOG_ERROR("failed to unregister connection handler for fd: " + shared::string::toString(fd) + ": " + std::string(e.what()));
			}
		}

		m_vServer.shutdown();
	}

	io::EventResult AcceptEventHandler::onReadable(int32_t) {
		LOG_CONTEXT("read event  | server: " + m_vServer.getVirtualServerInfo());

		while (Connection* conn = m_vServer.acceptNewConnection()) {
			m_dispatcher.registerHandler(conn->getSocket().getFd(),
										 new ConnectionEventHandler(m_vServer, *conn, m_dispatcher),
										 io::AMultiplexer::EVENT_ALL);
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult AcceptEventHandler::onWriteable(int32_t) {
		LOG_ERROR("write event | server: " + m_vServer.getVirtualServerInfo() + "unexpected because not registered");
		return io::UNREGISTER;
	}

	io::EventResult AcceptEventHandler::onHangup(int32_t) {
		LOG_ERROR("hangup event | server: " + m_vServer.getVirtualServerInfo());
		return io::UNREGISTER;
	}

	io::EventResult AcceptEventHandler::onError(int32_t) {
		LOG_ERROR("error event | server: " + m_vServer.getVirtualServerInfo() + "multiplexing error");
		return io::UNREGISTER;
	}

} /* namespace core */
