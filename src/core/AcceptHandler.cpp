#include "core/AcceptHandler.hpp"

#include "core/IOHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new AcceptHandler object.
	 */
	AcceptHandler::AcceptHandler(http::VirtualServer& vServer, Dispatcher& dispatcher)
		: IHandler()
		, m_vServer(vServer)
		, m_dispatcher(dispatcher) {
	}

	void AcceptHandler::handle(int32_t fd, uint32_t events) {
		if (EPOLLERR || EPOLLHUP) {
			throw std::runtime_error("listen socket error or hungup");
		}

		if (events & EPOLLIN) {
			while (m_vServer.addConnection()) {
				http::Connection& conn = m_vServer.getConnections().back();
				IHandler* handler = new IOHandler(m_vServer, conn);
				m_dispatcher.registerHandler(conn.getSocket().getFd(), EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP, handler);
			}
		}
	}

} /* namespace core */
