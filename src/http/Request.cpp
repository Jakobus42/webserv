#include "http/Request.hpp"

#include <string.h>

namespace http {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: m_read_buffer()
		, m_received_bytes(0)
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
	Request::Request(const Request &other)
		: m_read_buffer()
		, m_received_bytes(other.getReceivedBytes())
		, m_requestData(other.getRequestData())
		, m_status(other.m_status) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Request object to assign from.
	 * @return A reference to the assigned Request object.
	 */
	Request &Request::operator=(const Request &rhs) {
		if (this == &rhs)
			return *this;
		m_requestData = rhs.getRequestData();
		m_received_bytes = rhs.getReceivedBytes();
		m_status = rhs.m_status;
		bzero(m_read_buffer, sizeof(m_read_buffer));
		return *this;
	}

	uint32_t Request::getReceivedBytes(void) const {
		return m_received_bytes;
	};

	const t_requestData &Request::getRequestData(void) const {
		return m_requestData;
	};

	const RequestStatus &Request::getStatus(void) const {
		return m_status;
	};

	// 3 different things can occur:
	// 1. The request is not fully received
	// 2. The request is fully received and there is no leftover data
	// 3. The request is fully received and there is leftover data

	bool Request::parse(void) {
		std::string input;
		if (m_restData != "") {
			input = m_restData;
		}
		input += m_read_buffer;
		// Remove \r from input 
		for (int i = 0; i < input.size(); i++) 
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

} /* namespace http */
