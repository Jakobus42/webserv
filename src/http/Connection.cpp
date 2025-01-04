#include "http/Connection.hpp"

#include <unistd.h>

#include <cstring>

namespace http {

	/**
	 * @brief Constructs a new Connection object.
	 */
	Connection::Connection()
		: m_clientSocket()
		, m_requestBuffer()
		, m_responseBuffer()
		, m_byteBuffer() {
	}

	/**
	 * @brief Constructs a new Connection object.
	 */
	Connection::Connection(int listenSocket) throw(std::exception)
		: m_clientSocket(listenSocket)
		, m_requestBuffer()
		, m_responseBuffer()
		, m_byteBuffer() {
	}

	/**
	 * @brief Destroys the Connection object.
	 * @note Closes the file, since this is our only reference to it.
	 */
	Connection::~Connection() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Connection object to copy.
	 */
	Connection::Connection(const Connection& other)
		: m_clientSocket(other.getSocket())
		, m_requestBuffer(other.getRequestBuffer())
		, m_responseBuffer(other.getResponseBuffer())
		, m_byteBuffer() {
		std::memcpy(m_byteBuffer, other.getByteBuffer(), BUFFER_SIZE);
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Connection object to assign from.
	 * @return A reference to the assigned Connection object.
	 */
	Connection& Connection::operator=(const Connection& rhs) {
		if (this == &rhs)
			return *this;
		m_clientSocket.operator=(rhs.getSocket());
		m_requestBuffer = rhs.getRequestBuffer();
		return *this;
	}

	bool Connection::operator==(const Connection& other) const {
		return other.getSocket() == this->getSocket();
	}

	void Connection::close(void) {
		this->getSocket().close();
	}

	const ClientSocket& Connection::getSocket(void) const {
		return m_clientSocket;
	}

	ClientSocket& Connection::getSocket(void) {
		return m_clientSocket;
	}

	int Connection::getClientSocketFd(void) const {
		return m_clientSocket.getFd();
	}

	const http::Request& Connection::getRequestBuffer(void) const {
		return m_requestBuffer;
	}

	http::Request& Connection::getRequestBuffer(void) {
		return m_requestBuffer;
	}

	const http::Response& Connection::getResponseBuffer(void) const {
		return m_responseBuffer;
	}

	http::Response& Connection::getResponseBuffer(void) {
		return m_responseBuffer;
	}

	const char* Connection::getByteBuffer(void) const {
		return m_byteBuffer;
	}

	char* Connection::getByteBuffer(void) {
		return m_byteBuffer;
	}

} // namespace http
