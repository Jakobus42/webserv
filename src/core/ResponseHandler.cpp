#include "core/ResponseHandler.hpp"

#include <cstring>

namespace core {

	/**
	 * @brief Constructs a new ResponseHandler object.
	 */
	ResponseHandler::ResponseHandler() {
	}

	/**
	 * @brief Destroys the ResponseHandler object.
	 */
	ResponseHandler::~ResponseHandler() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other ResponseHandler object to copy.
	 */
	ResponseHandler::ResponseHandler(const ResponseHandler &) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other ResponseHandler object to assign from.
	 * @return A reference to the assigned ResponseHandler object.
	 */
	ResponseHandler &ResponseHandler::operator=(const ResponseHandler &) {
		return *this;
	}

	// note: the implementation is just temporary
	void ResponseHandler::handle(HandleContext &ctx) {
		int fd = ctx.conn.getSocket().getFd();
		std::cout << "ResponseHandler on fd: " << fd << std::endl;
		const char *response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 13\r\n"
			"\r\n"
			"Hello, World!";

		ssize_t bytesSent = send(fd, response, strlen(response), 0);
		close(fd);

		if (bytesSent < 0) {
			std::cerr << "Error sending data" << std::endl;
			return;
		}
		std::cout << "Sent response: " << response << std::endl;
	}

} /* namespace core */
