#pragma once

#include <stdint.h>

#include "shared/defines.hpp"

#include <string>
#include <vector>

#define BUFFER_SIZE 256

namespace http {

	typedef struct s_requestData {
			std::string method;
			std::string uri;
			std::string version;
			std::vector<std::string> headers;
			std::string body;
	} t_requestData;

	typedef struct parsingData {
			long pos;
			std::string rest;
	} t_parsingData;

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
			const t_requestData &getRequestData(void) const;
			const t_parsingData &getParsingData(void) const;
			const RequestStatus &getStatus(void) const;

			bool parse(void);
		private:
			char m_read_buffer[BUFFER_SIZE];
			uint32_t m_received_bytes;
			t_requestData m_requestData;
			t_parsingData m_parsingData;
			RequestStatus m_status;

			bool parseHead(void);
			bool parseHeaders(void);
			bool parseBody(void);
			// ...
	};

} /* namespace http */
