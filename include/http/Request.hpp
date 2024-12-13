#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#define BUFFER_SIZE 256

namespace http {

	/**
	 * @class Request
	 * @brief ...
	 */
	class Request {
		public:
			Request();
			~Request();
			Request(const Request& other);
			Request& operator=(const Request& rhs);

			uint32_t getReceivedBytes(void) const;
			const std::string& getData(void) const;
			const std::string& getHead(void) const;
			const std::string& getBody(void) const;

		private:
			char m_read_buffer[BUFFER_SIZE];
			uint32_t m_received_bytes;
			std::string m_data;
			std::string m_head;
			std::string m_body;
			// ...
	};

} /* namespace http */
