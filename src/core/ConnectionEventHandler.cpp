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
		, m_requestParser()
		, m_requestProcessor(vServer.getServerConfig(), dispatcher)
		, m_totalBytesSent(0)
		, m_requests()
		, m_responses() {
		m_requestProcessor.init();
		// todo: set max body size for parser
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

		shared::container::Buffer<http::RequestParser::BUFFER_SIZE>& buffer = m_requestParser.getReadBuffer();
		std::size_t available = buffer.prepareWrite();
		ssize_t bytesWritten = m_connection.recv(buffer.writePtr(), available);
		if (bytesWritten == -1) {
			LOG_ERROR("failed to receive data");
			return io::UNREGISTER;
		} else if (bytesWritten == 0) {
			return io::UNREGISTER;
		}
		buffer.advanceWriter(bytesWritten);
		LOG_INFO("received " + shared::string::toString(bytesWritten) + " bytes");

		m_connection.updateActivityTimestamp();

		if (!m_requestParser.parse()) {
			http::Request* req = m_requestParser.releaseRequest();

			if (req->hasHeader("keep-alive") && req->getHeader("keep-alive").front() == "close") {
				m_connection.setIsKeepAlive(false);
			}

			m_requests.push(req);
			m_requestParser.reset();
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionEventHandler::onWriteable(int32_t) {
		LOG_CONTEXT("write event | server: " + m_vServer.getVirtualServerInfo() +
					" | connection: " + m_connection.getConnectionInfo());

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
		}
		LOG_INFO("sent " + shared::string::toString(bytesSent) + " bytes");

		m_connection.updateActivityTimestamp();

		m_totalBytesSent += bytesSent;
		if (m_totalBytesSent == serializedResponse.size()) {
			m_totalBytesSent = 0;
			delete response;
			m_responses.pop();
			return m_connection.isKeepAlive() ? io::KEEP_MONITORING : io::UNREGISTER;
		}
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
