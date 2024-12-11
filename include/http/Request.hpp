#pragma once

#include <stdint.h>

#include "shared/defines.hpp"

#include <string>
#include <vector>
#include <map>

#define BUFFER_SIZE 256

namespace http {

	typedef struct s_requestData {
			std::string method;
			std::string uri;
			std::string version;
			std::map<std::string, std::vector<std::string> > headers;
			std::string body;
	} t_requestData;

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
			const RequestStatus &getStatus(void) const;

			bool parse(void);
		private:
			char m_read_buffer[BUFFER_SIZE];
			uint32_t m_received_bytes;
			std::string m_restData;
			t_requestData m_requestData;
			RequestStatus m_status;

			bool parseHead(std::string& input);
			bool parseHeaders(std::string& input);
			bool parseBody(std::string& input);
			// ...
	};

} /* namespace http */
