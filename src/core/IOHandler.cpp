#include "core/IOHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new IOHandler object.
	 */
	IOHandler::IOHandler()
		: AHandler() {
	}

	/**
	 * @brief Destroys the IOHandler object.
	 */
	IOHandler::~IOHandler() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other IOHandler object to copy.
	 */
	IOHandler::IOHandler(const IOHandler&)
		: AHandler() {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other IOHandler object to assign from.
	 * @return A reference to the assigned IOHandler object.
	 */
	IOHandler& IOHandler::operator=(const IOHandler&) {
		return *this;
	}

	void IOHandler::handle(HandlerContext& ctx) {
		if (ctx.events & EPOLLIN) {
			m_requestHandler.handle(ctx);
		}
		if (ctx.events & EPOLLOUT && m_requestHandler.completed()) {
			m_responseHandler.handle(ctx);
		}
		if (ctx.events & EPOLLERR) {
			std::cout << "oup oup oup" << std::endl;
		}
	}

	bool IOHandler::completed() const {
		return m_requestHandler.completed() && m_responseHandler.completed();
	}

	bool IOHandler::failed() const {
		return m_requestHandler.failed() || m_responseHandler.failed();
	}

} /* namespace core */
