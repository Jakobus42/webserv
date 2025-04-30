#include "core/GetRequestHandler.hpp"

#include "http/Request.hpp"
#include "http/Response.hpp"

namespace core {

	GetRequestHandler::GetRequestHandler()
		: ARequestHandler()
		, m_buffer(BUFFER_SIZE)
		, m_fileStream()
		, m_streamPosition(0) {}

	GetRequestHandler::~GetRequestHandler() {}

	// todo: make sure path exists, file exists, can access, etc. etc.
	// throw correct status code if any issues arise
	void GetRequestHandler::checkFileAccess() const throw(http::HttpException) {
	}

	void GetRequestHandler::openFile() {
		m_fileStream.open(m_filePath.c_str());
		if (!m_fileStream.is_open()) {
			throw http::HttpException(http::INTERNAL_SERVER_ERROR, "File " + m_filePath + " could not be opened");
		}
		m_fileStream.seekg(m_streamPosition);
	}

	bool GetRequestHandler::readFile(http::Response& response) {
		m_buffer.clear();
		m_fileStream.read(m_buffer.data(), BUFFER_SIZE);
		std::streamsize bytesRead = m_fileStream.gcount();
		m_fileStream.close();
		if (bytesRead > 0) {
			response.appendBody(shared::string::StringView(m_buffer.data()));
		} else {
			m_state = DONE;
			return false;
		}
		return true;
	}

	bool GetRequestHandler::handle(const http::Request&, http::Response& response) {
		switch (m_state) {
			case PREPROCESS: {
				checkFileAccess();
				m_state = PROCESS;
				return true;
			}
			case PROCESS: {
				openFile();
				readFile(response);
				return true;
			}
			case DONE: {
				return false;
			}
		}
		return false;
	}

	void GetRequestHandler::reset() {
		this->ARequestHandler::reset();
		m_buffer.clear();
		if (m_fileStream.is_open()) {
			m_fileStream.close();
		}
		m_fileStream.clear();
		m_streamPosition = 0;
	}

} /* namespace core */
