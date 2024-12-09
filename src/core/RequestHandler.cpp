#include "core/RequestHandler.hpp"

namespace core {

	/**
	 * @brief Constructs a new RequestHandler object.
	 */
	RequestHandler::RequestHandler() {
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
	RequestHandler::RequestHandler(const RequestHandler &other) {
		(void)other;
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other RequestHandler object to assign from.
	 * @return A reference to the assigned RequestHandler object.
	 */
	RequestHandler &RequestHandler::operator=(const RequestHandler &rhs) {
		(void)rhs;
		return *this;
	}

	// note: the implementation is just temporary
	void RequestHandler::handle(HandleContext &ctx) {
		int fd = ctx.conn.getSocket().getFd();
		char buffer[1024];

		ssize_t bytesReceived = recv(fd, buffer, sizeof(buffer) - 1, 0);
		if (bytesReceived < 0) {
			ctx.conn.close();
			std::cerr << "Error receiving data" << std::endl;
			return;
		}
		if (bytesReceived == 0) {
			ctx.conn.close(); // why can I close it in read and response? sus
			return;
		}
		buffer[bytesReceived] = '\0';
		std::cout << "Received bytes: " << bytesReceived << std::endl;
		std::cout << "Received data: " << buffer << std::endl;
	}

} /* namespace core */
