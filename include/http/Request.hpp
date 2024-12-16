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

	enum QuoteFlag {
		NO_QUOTE = 0,
		SINGLE_QUOTE = 1,
		DOUBLE_QUOTE = 2

	};

	enum ExpectedBody 
	{
		NO_BODY = 0,
		CHUNKED = 1,
		CONTENT_LENGTH = 2
	};

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
			const t_requestData &getRequestData(void) const;
			const RequestStatus &getStatus(void) const;

			void setReadBuffer(const char *buffer);
			void PrintRequestData();
			bool parse(void);
		private:
			char m_read_buffer[BUFFER_SIZE];
			uint32_t m_receivedBytes;
			uint32_t m_contentLength;
			std::string m_restData;
			t_requestData m_requestData;
			
			RequestStatus m_status;
			ExpectedBody m_expectedBody;
			
			bool parseHead(std::string& input);
			bool parseHeaders(std::string& input);
			bool parseBody(std::string& input);

			bool parseHeader(std::string &line);
			bool interpretHeaders();
			bool parseBodyChunked(std::string &input);

			// ...
	};

} /* namespace http */
