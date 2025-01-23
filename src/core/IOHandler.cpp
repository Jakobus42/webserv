#include "core/IOHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new IOHandler object.
	 */
	IOHandler::IOHandler(http::VirtualServer& vServer)
		: m_vServer(vServer) {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
	}

	void IOHandler::handle(int32_t fd, uint32_t events) {
		if (events & EPOLLERR || EPOLLHUP) {
			m_vServer.log("epoll error or hangup detected on client socket", shared::ERROR); // todo maybe more detailed logs but idc rn lol
			m_vServer.dropClient(fd);
			return this->markDone();
		}

		try {
			if (events & EPOLLIN) {
				this->handleRead(fd);
			}

			if (events & EPOLLOUT) {
				this->handleWrite(fd);
			}
		} catch (const std::exception& e) {
			m_vServer.log(e.what(), shared::ERROR);
			m_vServer.dropClient(fd);
			return this->markDone();
		}
	}

	void IOHandler::handleRead(int32_t) {
	}

	void IOHandler::handleWrite(int32_t) {
	}

} /* namespace core */
