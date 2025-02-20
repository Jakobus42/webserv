#include "core/EventHandler.hpp"

#include "core/VirtualServer.hpp"
#include "http/Connection.hpp"
#include "http/Request.hpp"
#include "http/VirtualServer.hpp"
#include "shared/Buffer.hpp"

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
	EventHandler::~EventHandler() {
		while (!m_requests.empty()) {
			delete m_requests.front();
			m_requests.pop();
		}
	}

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

	bool EventHandler::shouldDrop(void) const {
		// @TODO: implement depending on the event
		if (m_state == FAILED || m_state == COMPLETED)
			return true;
		return false;
	}

	// EVENT HANDLING LOGIC

	// @TODO: theoretically could already build a response
	// otherwise, we would always wait until we get EPOLLOUT until we start building
	// potentially wasting a lot of time while we're still in EPOLLIN
	void EventHandler::handleRequest(void) {
		shared::Buffer<BUFFER_SIZEE>& buff = m_reqParser.getWriteBuffer();

		std::cout << "reading" << std::endl;
		std::size_t available = buff.prepareWrite();
		ssize_t bytesReceived = m_connection.getSocket().recv(buff.getWritePos(), available);
		if (bytesReceived < 0) {
			setState(FAILED);
			throw std::runtime_error("recv() failed");
		}
		if (bytesReceived == 0) {
			setState(COMPLETED);
			return;
		}
		buff.advanceWriter(bytesReceived);

		m_reqParser.process();
		if (m_reqParser.isComplete()) {
			m_requests.push(m_reqParser.releaseRequest());
			m_reqParser.reset();
			std::cout << "parsed: " << std::endl
					  << m_requests.back()->toString() << std::endl;
		}
	}

	void EventHandler::buildResponse(void) {
		if (m_requests.empty() || m_state > WRITING)
			return;
		const http::Request* currentRequest = m_requests.front();
		http::Response& currentResponse = m_connection.getResponseBuffer();
		currentResponse.doMagicToCalculateStatusCode(*currentRequest);

		// set response type: CGI, error, or normal
		// i.e. look for file and error if not found, otherwise build that
		// or, if cgi header found, validate, run and respond accordingly

		switch (currentResponse.getType()) {
			case (http::IDK_NORMAL_I_GUESS):
				currentResponse.buildFromRequest(*currentRequest);
				break;
			case (http::CGI):
				currentResponse.buildCGIResponse(*currentRequest);
				break;
			case (http::ERROR):
				currentResponse.buildErrorResponse(*currentRequest);
				break;
		}
		if (currentResponse.done()) {
			m_responses.push(currentResponse);
			delete currentRequest;
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

		ssize_t bytesSent = m_connection.getSocket().send(currentResponse.getRawResponse().c_str(), currentResponse.getRawResponse().length());
		if (bytesSent < 0) {
			std::cerr << "Error sending data" << std::endl;
		} else {
			setState(COMPLETED);
			currentResponse.reset();
			m_responses.pop();
		}
		std::cout << "m_responses now " << m_responses.size() << " long" << std::endl;
	}

	void EventHandler::handleError(void) {
		// ...
	}

	void EventHandler::killSelf(void) {
		m_server.removeConnection(m_connection);
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


} /* namespace core */
