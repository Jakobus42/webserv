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

	void AcceptHandler::handle(int32_t, uint32_t events) {
		if (events & EPOLLERR) {
			throw std::runtime_error("listen socket error: EPOLLERR detected.");
		} else if (events & EPOLLHUP) {
			throw std::runtime_error("listen socket error: EPOLLHUP detected (socket hangup).");
		}

		if (events & EPOLLIN) {
			while (m_vServer.acceptClient()) {
				IHandler* handler = new IOHandler(m_vServer);
				m_dispatcher.registerHandler(m_vServer.getClients().back(), EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP, handler);
			}
		}
	}

} /* namespace core */
