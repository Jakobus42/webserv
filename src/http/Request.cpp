#include "http/Request.hpp"

#include <string.h>

namespace http {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: m_read_buffer()
		, m_receivedBytes(0)
		, m_requestData()
		, m_status(PARSE_START) {}

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
		, m_requestData(other.getRequestData())
		, m_status(other.m_status) {}

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
		bzero(m_read_buffer, sizeof(m_read_buffer));
		return *this;
	}

	uint32_t Request::getReceivedBytes(void) const {
		return m_receivedBytes;
	};

	const t_requestData &Request::getRequestData(void) const {
		return m_requestData;
	};

	const RequestStatus &Request::getStatus(void) const {
		return m_status;
	};

	bool Request::parse(void) {
		std::string input;
		if (m_restData != "") {
			input = m_restData;
			m_restData = "";
		}
		input += m_read_buffer;
		std::cout << "input: " << input << std::endl;
		//Remove \r from input & replace tabs with spaces
		for (unsigned long i = 0; i < input.size(); i++) 
		{
			if (input[i] == '\r') 
			{
				if (input[i + 1] == '\n') {
					input.erase(i, 1);
				}
			}
			if (input[i] == '	') 
			{
				input[i] = ' ';
			}
		}	
		if (m_status == PARSE_HEAD || m_status == PARSE_START) {
			if (!parseHead(input))
				return false;
		}
		if (m_status == PARSE_HEADERS) {
			if (!parseHeaders(input))
				return false;
		}
		if (m_status == PARSE_BODY) {
			if (!parseBody(input))
				return false;
		}
		if (m_status == PARSE_END) {
			//Parsing done
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
	void Request::setReadBuffer(const char *buffer) {
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
