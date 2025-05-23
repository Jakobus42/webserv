#include "core/ConnectionEventHandler.hpp"

#include "core/Connection.hpp"
#include "core/VirtualServer.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

namespace core {

	ConnectionEventHandler::ConnectionEventHandler(VirtualServer& vServer, Connection& conn, io::Dispatcher& dispatcher)
		: m_vServer(vServer)
		, m_connection(conn)
		, m_requestParser(http::RequestParserConfig(vServer.getServerConfigs().front().global))
		, m_requestProcessor(vServer.getServerConfigs(), dispatcher)
		, m_totalBytesSent(0)
		, m_requests()
		, m_responses() {
		m_requestProcessor.init();
	}

	ConnectionEventHandler::~ConnectionEventHandler() {
		LOG_CONTEXT("server: " + m_vServer.getVirtualServerInfo() +
					" | connection: " + m_connection.getConnectionInfo());

		while (!m_requests.empty()) {
			delete m_requests.front();
			m_requests.pop();
		}

		while (!m_responses.empty()) {
			delete m_responses.front();
			m_responses.pop();
		}

		m_vServer.removeConnection(m_connection);
	}

	io::EventResult ConnectionEventHandler::onReadable(int32_t) {
		LOG_CONTEXT("read event  | server: " + m_vServer.getVirtualServerInfo() +
					" | connection: " + m_connection.getConnectionInfo());

		if (m_connection.hasTimedOut()) {
			LOG_INFO("connection timed out");
			return io::UNREGISTER;
		}

		shared::container::Buffer<http::RequestParser::BUFFER_SIZE>& buffer = m_requestParser.getReadBuffer();
		std::size_t available = buffer.prepareWrite();
		ssize_t bytesReceived = m_connection.recv(buffer.writePtr(), available);
		if (bytesReceived == -1) {
			LOG_ERROR("failed to receive data");
			return io::UNREGISTER;
		} else if (bytesReceived == 0) {
			return io::UNREGISTER;
		}
		buffer.advanceWriter(bytesReceived);
		LOG_INFO("received " + shared::string::toString(bytesReceived) + " bytes");

		m_connection.updateActivityTimestamp();

		if (!m_requestParser.parse()) {
			http::Request* req = m_requestParser.releaseRequest();

			m_requests.push(req);
			m_requestParser.reset();
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionEventHandler::onWriteable(int32_t) {
		LOG_CONTEXT("write event | server: " + m_vServer.getVirtualServerInfo() +
					" | connection: " + m_connection.getConnectionInfo());

		if (m_connection.hasTimedOut()) {
			LOG_INFO("connection timed out");
			return io::UNREGISTER;
		}

		if (!m_requests.empty()) {
			http::Request* request = m_requests.front();
			if (m_requestProcessor.processRequest(*request)) {
				return io::KEEP_MONITORING;
			}
			m_responses.push(m_requestProcessor.releaseResponse());
			m_requestProcessor.reset();

			delete request;
			m_requests.pop();
		}

		if (m_responses.empty()) {
			return io::KEEP_MONITORING;
		}


		http::Response* response = m_responses.front();
		const std::string& serializedResponse = response->serialize();
		ssize_t bytesSent = m_connection.send(serializedResponse.c_str() + m_totalBytesSent, serializedResponse.size() - m_totalBytesSent);
		if (bytesSent == -1) {
			LOG_ERROR("failed to send data");
			return io::UNREGISTER;
		} else if (bytesSent == 0) {
			m_totalBytesSent = 0;

			if (response->hasHeader("Connection") && response->getHeader("Connection").front() == "close") {
				m_connection.setIsKeepAlive(false);
			}
			delete response;
			m_responses.pop();

			return m_connection.isKeepAlive() ? io::KEEP_MONITORING : io::UNREGISTER;
		}
		LOG_INFO("sent " + shared::string::toString(bytesSent) + " bytes");
		m_totalBytesSent += bytesSent;

		m_connection.updateActivityTimestamp();

		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionEventHandler::onHangup(int32_t) {
		LOG_ERROR("hangup event | virtual server: " + m_vServer.getVirtualServerInfo() +
				  " | connection: " + m_connection.getConnectionInfo());
		return io::UNREGISTER;
	}

	io::EventResult ConnectionEventHandler::onError(int32_t) {
		LOG_ERROR("error event | virtual server: " + m_vServer.getVirtualServerInfo() +
				  " | connection: " + m_connection.getConnectionInfo() + " multiplexing error");
		return io::UNREGISTER;
	}

} /* namespace core */
