#include "http/Request.hpp"

#include <string.h>

namespace http {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: m_receivedBytes(0)
		, m_contentLength(0)
		, m_restData("")
		, m_requestData()
		, m_status(PARSE_START)
		, m_expectedBody(NO_BODY)
		, m_chunkedStatus(CHUNK_SIZE) {}

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
		: m_receivedBytes(other.getReceivedBytes())
		, m_contentLength(other.getContentLength())
		, m_restData(other.getRestData())
		, m_requestData(other.getRequestData())
		, m_status(other.m_status)
		, m_expectedBody(other.getExpectedBody())
		, m_chunkedStatus(other.getChunkedStatus()) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Request object to assign from.
	 * @return A reference to the assigned Request object.
	 */
	Request& Request::operator=(const Request& rhs) {
		if (this == &rhs)
			return *this;
		m_receivedBytes = rhs.getReceivedBytes();
		m_contentLength = rhs.getContentLength();
		m_restData = rhs.getRestData();
		m_requestData = rhs.getRequestData();
		m_status = rhs.getStatus();
		m_expectedBody = rhs.getExpectedBody();
		m_chunkedStatus = rhs.getChunkedStatus();
		return *this;
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

	const ChunkedStatus& Request::getChunkedStatus(void) const {
		return m_chunkedStatus;
	};

	/**
	 * @brief Parses the request.
	 * @return True if the request was parsed successfully, false otherwise.
	 */
	bool Request::parse(char buffer[BUFFER_SIZE]) {
		std::string input;

		if (m_restData != "") {
			input = m_restData;
			m_restData = "";
		}
		input += buffer;
		if ((m_status == PARSE_HEAD || m_status == PARSE_START) && !parseHead(input)) {
			std::cout << "first oof" << std::endl;
			return false;
		}
		if (m_status == PARSE_HEADERS && !parseHeaders(input, HEADER)) {
			std::cout << "second oof" << std::endl;
			return false;
		}
		if (m_status == PARSE_BODY && !parseBody(input)) {
			std::cout << "third oof" << std::endl;
			return false;
		}
		if (input != "") {
			m_restData = input;
		}
		std::cout << "Request.parse successful" << std::endl;
		return true;
	}

	void Request::reset(void) {
		m_receivedBytes = 0;
		m_contentLength = 0;
		m_restData = "";
		m_requestData.method = "";
		m_requestData.uri = "";
		m_requestData.version = "";
		(void)m_requestData.headers.empty();
		m_requestData.body = "";
		(void)m_requestData.trailingHeaders.empty();
		m_status = PARSE_START;
	}

	void Request::PrintRequestData() {
		std::cout << "Method: " << m_requestData.method << std::endl;
		std::cout << "URI: " << m_requestData.uri << std::endl;
		std::cout << "Version: " << m_requestData.version << std::endl;
		std::cout << "------------------------------" << std::endl;
		std::cout << "Headers: " << std::endl;
		for (std::map<std::string, std::vector<std::string> >::const_iterator it = m_requestData.headers.begin(); it != m_requestData.headers.end(); ++it) {
			std::cout << it->first << ": ";
			for (std::vector<std::string>::const_iterator val = it->second.begin(); val != it->second.end(); ++val) {
				std::cout << *val << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "------------------------------" << std::endl;
		std::cout << "Trailing headers: " << std::endl;
		for (std::map<std::string, std::vector<std::string> >::const_iterator it = m_requestData.trailingHeaders.begin(); it != m_requestData.trailingHeaders.end(); ++it) {
			std::cout << it->first << ": ";
			for (std::vector<std::string>::const_iterator val = it->second.begin(); val != it->second.end(); ++val) {
				std::cout << *val << " ";
			}
			std::cout << std::endl;
		}
		std::cout << "------------------------------" << std::endl;
		std::cout << "Body: " << m_requestData.body << std::endl;
		std::cout << "------------------------------" << std::endl;
		std::cout << "Status: " << m_status << std::endl;
	}

	GetLineStatus Request::getNextLineHTTP(std::string& input, std::string& line) {
		for (unsigned long i = 0; i < input.size(); i++) {
			if (input[i] == '\r') {
				if (i != input.length() - 1 && input[i + 1] != '\n') {
					std::cout << "Error: Invalid line ending" << std::endl;
					;
					return GET_LINE_ERROR;
				}
			}
			if (input[i] == '\n') {
				if (input[i - 1] != '\r') {
					std::cout << "Error: Invalid line ending" << std::endl;
					return GET_LINE_ERROR;
				}
				line = input.substr(0, i - 1);
				input = input.substr(i + 1);
				return GET_LINE_OK;
			}
		}
		return GET_LINE_END;
	}

} /* namespace http */
