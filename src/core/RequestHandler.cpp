#include "core/RequestHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new RequestHandler object.
	 */
	RequestHandler::RequestHandler(const HandlerContext& ctx)
		: AHandler(ctx) {
	}

	/**
	 * @brief Destroys the RequestHandler object.
	 */
	RequestHandler::~RequestHandler() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other RequestHandler object to copy.
	 */
	RequestHandler::RequestHandler(const RequestHandler& other)
		: AHandler(other.getContext()) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other RequestHandler object to assign from.
	 * @return A reference to the assigned RequestHandler object.
	 */
	RequestHandler& RequestHandler::operator=(const RequestHandler& rhs) {
		if (this == &rhs)
			return *this;
		this->AHandler::operator=(rhs);
		return *this;
	}

	bool RequestHandler::shouldDrop(void) const {
		// if has "Connection: close" header and status is COMPLETED
		// also, if timeout done
		if (m_state == FAILED)
			return true;
		// if (m_state == COMPLETED && getHeadersOfRequestThisHandlerIsCurrentlyHandling().at("Connection").is("close"))
		// 	return true;
		return false;
	}

	// note: the implementation is just temporary
	// check for dropped connection, then ensure handler is unregistered
	void RequestHandler::handle() {
		int fd = m_context.conn.getSocket().getFd();

		ssize_t bytesReceived = recv(fd, m_context.conn.getBuffer(), BUFFER_SIZE - 1, 0);
		if (bytesReceived < 0) {
			// if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// 	return;
			// 	// hmm
			// }
			setState(FAILED); // just temporary
			// std::cerr << "Error receiving data for " << fd << ": " << errno << std::endl;
			std::cout << "Request FAILED somehow" << std::endl;
			static int i = 0;
			i++;
			if (i >= 2)
				throw std::runtime_error("recv failed twice on the same connection - this shouldn't happen");
			return;
		}
		if (bytesReceived == 0) {
			setState(COMPLETED); // connection closed
			std::cout << "Request COMPLETED after reading 0" << std::endl;
			return;
		}
		m_context.conn.getBuffer()[bytesReceived] = '\0';
		std::cout << "Received bytes: " << bytesReceived << std::endl;
		std::cout << "Received data: " << m_context.conn.getBuffer() << std::endl;
		if (!m_context.conn.getRequest().parse(m_context.conn.getBuffer())) {
			setState(FAILED);
			// handle failure
			std::cout << "Request FAILED" << std::endl;
			return;
		}
		if (m_context.conn.getRequest().getStatus() == http::PARSE_END) {
			std::cout << "Request COMPLETED right after parsing" << std::endl;
			setState(COMPLETED);
		} else
			setState(PROCESSING);
		// if request.done() then setState(COMPLETED);
	}

} /* namespace core */
