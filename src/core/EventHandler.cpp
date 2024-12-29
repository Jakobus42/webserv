#include "core/EventHandler.hpp"

#include "http/Connection.hpp"
#include "http/Request.hpp"
#include "http/VirtualServer.hpp"

#include <cstring>

namespace core {

	/**
	 * @brief Constructs a new EventHandler object.
	 */
	EventHandler::EventHandler(http::VirtualServer& server, http::Connection& connection, uint32_t events)
		: m_events(events)
		, m_state(PENDING_READ)
		, m_requests()
		, m_connection(connection)
		, m_server(server) {}

	/**
	 * @brief Destroys the EventHandler object.
	 */
	EventHandler::~EventHandler() {}

	/**
	 * @brief Copy constructor.
	 * @param other The other EventHandler object to copy.
	 */
	EventHandler::EventHandler(const EventHandler& other)
		: m_events(other.getEvents())
		, m_state(other.getState())
		, m_requests(other.getRequests())
		, m_connection(other.getConnection())
		, m_server(other.getServer()) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other EventHandler object to assign from.
	 * @return A reference to the assigned EventHandler object.
	 */
	EventHandler& EventHandler::operator=(const EventHandler& rhs) {
		if (this == &rhs)
			return *this;
		m_events = rhs.getEvents();
		m_state = rhs.getState();
		m_requests = rhs.getRequests();
		m_connection = rhs.getConnection();
		m_server = rhs.getServer();
		return *this;
	}

	std::queue<http::Request>& EventHandler::getRequests(void) {
		return m_requests;
	}

	const std::queue<http::Request>& EventHandler::getRequests(void) const {
		return m_requests;
	}

	HandlerState EventHandler::getState(void) const {
		return m_state;
	}

	http::VirtualServer& EventHandler::getServer(void) {
		return m_server;
	}

	http::VirtualServer& EventHandler::getServer(void) const {
		return m_server;
	}

	http::Connection& EventHandler::getConnection(void) {
		return m_connection;
	}

	http::Connection& EventHandler::getConnection(void) const {
		return m_connection;
	}

	uint32_t EventHandler::getEvents(void) const {
		return m_events;
	}

	void EventHandler::setState(HandlerState newState) {
		m_state = newState;
	}

	bool EventHandler::shouldDrop(void) const {
		// @TODO: implement depending on the event
		if (m_state == FAILED)
			return true;
		if (m_state == COMPLETED)
			return true;
		return false;
	}

	// EVENT HANDLING LOGIC

	// @TODO: theoretically could already build a response
	// otherwise, we would always wait until we get EPOLLOUT until we start building
	// potentially wasting a lot of time while we're still in EPOLLIN
	void EventHandler::handleRequest(void) {
		int fd = m_connection.getClientSocketFd();
		char* buffer = m_connection.getBuffer();

		ssize_t bytesReceived = recv(fd, buffer, BUFFER_SIZE - 1, 0);
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
		buffer[bytesReceived] = '\0';
		std::cout << "Received bytes: " << bytesReceived << std::endl;
		std::cout << "Received data: " << buffer << std::endl;
		if (!m_connection.getRequest().parse(buffer)) {
			setState(FAILED);
			// handle failure
			std::cout << "Request FAILED" << std::endl;
			return;
		}
		if (m_connection.getRequest().getStatus() == http::PARSE_END) {
			std::cout << "Request COMPLETED right after parsing" << std::endl;
			setState(WAITING_FOR_WRITE);
		} else
			setState(PROCESSING);
		// if request.done() then setState(COMPLETED);
	}

	void EventHandler::handleResponse(void) {
		int fd = m_connection.getSocket().getFd();
		std::cout << "handleResponse on fd: " << fd << std::endl;
		const char* response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 13\r\n"
			"\r\n"
			"Hello, World!";

		ssize_t bytesSent = send(fd, response, strlen(response), 0);
		if (bytesSent < 0) {
			std::cerr << "Error sending data" << std::endl;
		} else {
			std::cout << "Sent response: " << response << std::endl;
			(void)response;
			m_connection.getRequest().reset();
			setState(COMPLETED);
			std::cout << "Response set to COMPLETED" << std::endl;
		}
	}

	void EventHandler::handleError(void) {
		// ...
	}

	void EventHandler::killSelf(void) {
		m_server.removeConnection(m_connection);
	}

} /* namespace core */
