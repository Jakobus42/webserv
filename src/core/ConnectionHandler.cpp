#include "core/ConnectionHandler.hpp"

#include "core/Connection.hpp"
#include "core/VirtualServer.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/Logger.hpp"
#include "shared/stringUtils.hpp"

namespace core {

	ConnectionHandler::ConnectionHandler(VirtualServer* vServer, Connection* conn)
		: m_vServer(vServer)
		, m_connection(conn)
		, m_requestParser()
		, m_totalBytesSent(0)
		, m_requests()
		, m_responses() {
		// todo: set max body size for parser
	}

	ConnectionHandler::~ConnectionHandler() {
		while (!m_requests.empty()) {
			delete m_requests.front();
			m_requests.pop();
		}

		while (!m_responses.empty()) {
			delete m_responses.front();
			m_responses.pop();
		}
	}

	io::EventResult ConnectionHandler::onReadable(int32_t) {
		LOG_CONTEXT("read event | server: " + m_vServer->getVirtualServerInfo() +
					" | connection: " + m_connection->getConnectionInfo());

		shared::container::Buffer<http::RequestParser::BUFFER_SIZE>& buffer = m_requestParser.getReadBuffer();
		std::size_t available = buffer.prepareWrite();
		ssize_t bytesWritten = m_connection->recv(buffer.writePtr(), available);
		if (bytesWritten == -1) {
			LOG_ERROR("failed to receive data");
			return unregister();
		} else if (bytesWritten == 0) {
			return unregister();
		}
		m_connection->updateActivityTimestamp();

		buffer.advanceWriter(bytesWritten);
		LOG_INFO("received " + shared::string::toString(bytesWritten) + " bytes");

		if (m_requestParser.parse()) {
			http::Request* req = m_requestParser.releaseRequest();

			if (req->hasHeader("keep-alive") && req->getHeader("keep-alive").front() == "close") {
				m_connection->setIsKeepAlive(false);
			}

			m_requests.push(req);
			m_requestParser.reset();
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionHandler::onWriteable(int32_t) {
		LOG_CONTEXT("write event | server: " + m_vServer->getVirtualServerInfo() +
					" | connection: " + m_connection->getConnectionInfo());

		if (!m_requests.empty()) {
			http::Response* response = m_requestProcessor.processRequest(m_requests.front());
			if (response) {
				m_responses.push(response);
				delete m_requests.front();
				m_requests.pop();
			} else {
				return io::KEEP_MONITORING;
			}
		}

		if (m_responses.empty()) {
			return io::KEEP_MONITORING;
		}
		m_connection->updateActivityTimestamp();

		http::Response* response = m_responses.front();
		response->appendHeader("Content-Length", "0"); // tmp
		const std::string& serializedResponse = response->serialize();
		ssize_t bytesSent = m_connection->send(serializedResponse.c_str() + m_totalBytesSent, serializedResponse.size() - m_totalBytesSent);
		if (bytesSent == -1) {
			LOG_ERROR("failed to send data");
			return unregister();
		}
		LOG_INFO("sent " + shared::string::toString(bytesSent) + " bytes");

		m_totalBytesSent += bytesSent;
		if (m_totalBytesSent == serializedResponse.size()) {
			m_totalBytesSent = 0;
			delete response;
			m_responses.pop();
			return m_connection->isKeepAlive() ? io::KEEP_MONITORING : unregister();
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult ConnectionHandler::onError(int32_t) {
		LOG_CONTEXT("error event | virtual server: " + m_vServer->getVirtualServerInfo() +
					" | connection: " + m_connection->getConnectionInfo());
		return unregister();
	}

	io::EventResult ConnectionHandler::unregister() {
		m_vServer->removeConnection(m_connection);
		return io::UNREGISTER;
	}

} /* namespace core */
