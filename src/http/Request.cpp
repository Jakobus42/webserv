#include "http/Request.hpp"

#include <string.h>

namespace http {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: m_read_buffer()
		, m_receivedBytes(0)
		, m_contentLength(0)
		, m_restData("")
		, m_requestData()
		, m_status(PARSE_START)
		, m_expectedBody(NO_BODY) {}

	/**
	 * @brief Destroys the Request object.
	 */
	Request::~Request() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Request object to copy.
	 */
	Request::Request(const Request& other)
		: m_read_buffer()
		, m_receivedBytes(other.getReceivedBytes())
		, m_contentLength(other.getContentLength())
		, m_restData(other.getRestData())
		, m_requestData(other.getRequestData())
		, m_status(other.m_status)
		, m_expectedBody(other.getExpectedBody()) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Request object to assign from.
	 * @return A reference to the assigned Request object.
	 */
	Request& Request::operator=(const Request& rhs) {
		if (this == &rhs)
			return *this;
		m_requestData = rhs.getRequestData();
		m_receivedBytes = rhs.getReceivedBytes();
		m_status = rhs.m_status;
		return *this;
	}

	const char* Request::getReadBuffer(void) const {
		return m_read_buffer;
	}

	uint32_t Request::getReceivedBytes(void) const {
		return m_receivedBytes;
	};

	const uint32_t& Request::getContentLength(void) const {
		return m_contentLength;
	}

	const std::string& Request::getRestData(void) const {
		return m_restData;
	}

	const t_requestData& Request::getRequestData(void) const {
		return m_requestData;
	};

	const RequestStatus& Request::getStatus(void) const {
		return m_status;
	};

	const ExpectedBody& Request::getExpectedBody(void) const {
		return m_expectedBody;
	};

	/**
	 * @brief Parses the request.
	 * @return True if the request was parsed successfully, false otherwise.
	 */
	bool Request::parse(void) {
		std::string input;
		if (m_restData != "") {
			input = m_restData;
			m_restData = "";
		}
		input += m_read_buffer;
		std::cout << "input: " << input << std::endl;
		if (m_status == PARSE_HEAD || m_status == PARSE_START) {
			if (!parseHead(input)) {
				// ERROR MESSAGE
				return false;
			}
		}
		if (m_status == PARSE_HEADERS) {
			if (!parseHeaders(input)) {
				// ERROR MESSAGE
				return false;
			}
		}
		if (m_status == PARSE_BODY) {
			if (!parseBody(input)) {
				// ERROR MESSAGE
				return false;
			}
		}
		if (m_status == PARSE_END) {
			// Parsing done
		}
		if (input != "") {
			m_restData = input;
		}
		return true;
	}

	/**
	 * @brief Sets the read buffer.
	 * @param buffer The buffer to set.
	 */
	void Request::setReadBuffer(const char* buffer) {
		memcpy(m_read_buffer, buffer, BUFFER_SIZE);
	}

	void Request::PrintRequestData() {
		std::cout << "Method: " << m_requestData.method << std::endl;
		std::cout << "URI: " << m_requestData.uri << std::endl;
		std::cout << "Version: " << m_requestData.version << std::endl;
		std::cout << "Headers: " << std::endl;
		for (std::map<std::string, std::vector<std::string> >::const_iterator it = m_requestData.headers.begin(); it != m_requestData.headers.end(); ++it) {
			std::cout << it->first << ": ";
			for (std::vector<std::string>::const_iterator val = it->second.begin(); val != it->second.end(); ++val) {
				std::cout << *val << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "Body: " << m_requestData.body << std::endl;
	}

} /* namespace http */
