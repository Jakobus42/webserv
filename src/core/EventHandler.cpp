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
		, m_responses()
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
		, m_responses(other.getResponses())
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
		m_responses = rhs.getResponses();
		m_connection = rhs.getConnection();
		m_server = rhs.getServer();
		return *this;
	}

	t_requests& EventHandler::getRequests(void) {
		return m_requests;
	}

	const t_requests& EventHandler::getRequests(void) const {
		return m_requests;
	}

	t_responses& EventHandler::getResponses(void) {
		return m_responses;
	}

	const t_responses& EventHandler::getResponses(void) const {
		return m_responses;
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
		http::Request& request = m_connection.getRequest(); // @TODO: ensure this always works on the right (foremost) request

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
		if (!request.parse(buffer)) {
			setState(FAILED);
			// handle failure
			std::cout << "Request FAILED" << std::endl;
			return;
		}
		if (request.getStatus() == http::PARSE_END) {
			std::cout << "Request COMPLETED right after parsing" << std::endl;
			m_requests.push(request);
			request.reset();
			setState(WAITING_FOR_WRITE);
		} else
			setState(PROCESSING);
		// if request.done() then setState(COMPLETED);
		std::cout << "m_responses now " << m_responses.size() << " long" << std::endl;
	}

	void EventHandler::buildResponse(void) {
		if (m_requests.empty() || m_state > WRITING)
			return;
		http::Response& currentResponse = m_connection.getResponse();
		const std::string response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 13\r\n"
			"\r\n"
			"Hello, World!";

		currentResponse.setRawResponse(response);
		if (currentResponse.done()) {
			m_responses.push(currentResponse);
			m_requests.pop();
			setState(PENDING_SEND);
			currentResponse.reset();
		}
		std::cout << "m_requests now " << m_requests.size() << " long" << std::endl;
	}

	// take the topmost request from the queue and build a response
	void EventHandler::sendResponse(void) {
		if (m_responses.empty() || m_state < PENDING_SEND)
			return;
		setState(SENDING);
		http::Response& currentResponse = m_responses.front();
		int fd = m_connection.getClientSocketFd();
		std::cout << "sendResponse on fd: " << fd << std::endl;

		ssize_t bytesSent = send(fd, currentResponse.getRawResponse().c_str(), currentResponse.getRawResponse().length(), 0);
		if (bytesSent < 0) {
			std::cerr << "Error sending data" << std::endl;
		} else {
			std::cout << "Sent response: " << currentResponse.getRawResponse() << std::endl;
			setState(COMPLETED);
			currentResponse.reset();
			m_responses.pop();
			std::cout << "Response set to COMPLETED" << std::endl;
		}
		std::cout << "m_responses now " << m_responses.size() << " long" << std::endl;
	}

	void EventHandler::handleError(void) {
		// ...
	}

	void EventHandler::killSelf(void) {
		m_server.removeConnection(m_connection);
	}

} /* namespace core */
