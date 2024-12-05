#pragma once

#include <stdint.h>

#include "defines.hpp"

#include <string>

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
			Request(const Request &other);
			Request &operator=(const Request &rhs);

			uint32_t getReceivedBytes(void) const;
			http::Method getMethod(void) const;
			const std::string &getUri(void) const;
			const std::string &getData(void) const;
			const std::string &getHead(void) const;
			const std::string &getBody(void) const;
		private:
			char m_read_buffer[BUFFER_SIZE];
			uint32_t m_received_bytes;

			Method m_method;
			std::string m_uri;
			std::string m_data;
			std::string m_head; // TODO: make a map of headers or a header class
			std::string m_body;
			// ...
	};

} /* namespace http */
