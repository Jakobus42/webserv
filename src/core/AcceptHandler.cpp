#include "core/AcceptHandler.hpp"

#include "core/IOHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new AcceptHandler object.
	 */
	AcceptHandler::AcceptHandler(http::VirtualServer& vServer, Dispatcher& dispatcher)
		: AHandler()
		, m_vServer(vServer)
		, m_dispatcher(dispatcher) {
	}

	void AcceptHandler::handle(int32_t, uint32_t events) {
		if (events & EPOLLHUP || events & EPOLLERR) {
			m_vServer.log("unexpected epoll event on listen socket", shared::ERROR);
			m_vServer.shutDown();
			return this->markDone();
		}

		try {
			while (m_vServer.acceptClient()) {
				int32_t clientSocket = m_vServer.getClients().back();
				AHandler* handler = new IOHandler(m_vServer);
				m_dispatcher.registerHandler(clientSocket, EPOLLIN | EPOLLOUT, handler);
			}
		} catch (const std::exception& e) {
			m_vServer.log(e.what(), shared::ERROR);
			m_vServer.shutDown();
			return this->markDone();
		}
	}


} /* namespace core */
