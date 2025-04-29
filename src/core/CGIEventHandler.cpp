#include "core/CGIEventHandler.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/Logger.hpp"

#include <cstring>
#include <iostream>

namespace core {

	CGIEventHandler::CGIEventHandler(CGIProcessor& processor, const http::Request& request, http::Response*& response)
		: m_processor(processor)
		, m_request(request)
		, m_response(response)
		, m_bytesWritten(0)
		, m_responseParser() {
	}

	CGIEventHandler::~CGIEventHandler() {}

	io::EventResult CGIEventHandler::onReadable(int32_t fd) {
		shared::container::Buffer<http::ResponseParser::BUFFER_SIZE>& buffer = m_responseParser.getReadBuffer();

		std::size_t available = buffer.prepareWrite();
		ssize_t bytesRead = read(fd, buffer.writePtr(), available);
		if (bytesRead == -1) {
			LOG_ERROR("failed to read from CGI script");
			m_processor.notifyIOError();
			return io::UNREGISTER;
		} else if (bytesRead == 0 && m_responseParser.isComplete() == false) {
			LOG_ERROR("incomplete CGI response");
			m_processor.notifyIOError();
			return io::UNREGISTER;
		}
		buffer.advanceWriter(bytesRead);

		if (!m_responseParser.parse()) {
			m_response = m_responseParser.releaseResponse();
			if (m_response->isValid() == false) {
				m_processor.notifyIOError();
			} else {
				m_processor.notifyIOReadCompletion();
			}
			return io::UNREGISTER;
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult CGIEventHandler::onWriteable(int32_t fd) {
		ssize_t bytesWritten = write(fd,
									 m_request.getBody().c_str() + m_bytesWritten,
									 m_request.getBody().size() - m_bytesWritten);

		if (bytesWritten == -1) {
			LOG_ERROR("failed to write to CGI script");
			m_processor.notifyIOError();
			return io::UNREGISTER;
		}

		m_bytesWritten += bytesWritten;
		if (m_bytesWritten >= m_request.getBody().size()) {
			m_processor.notifyIOWriteCompletion();
			return io::UNREGISTER;
		}
		return io::KEEP_MONITORING;
	}

	io::EventResult CGIEventHandler::onHangup(int32_t) {
		LOG_ERROR("IO event hangup on CGI handler");
		m_processor.notifyIOError();
		return io::UNREGISTER;
	}

	io::EventResult CGIEventHandler::onError(int32_t) {
		LOG_ERROR("IO event error on CGI handler");
		m_processor.notifyIOError();
		return io::UNREGISTER;
	}

} /* namespace core */
