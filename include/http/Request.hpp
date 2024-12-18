#pragma once

#include <stdint.h>

#include "shared/defines.hpp"
#include "shared/stringUtils.hpp"

#include <limits>
#include <map>
#include <string>
#include <vector>

#define BUFFER_SIZE 256

namespace http {

	typedef struct s_requestData {
			std::string method;
			std::string uri;
			std::string version;
			std::map<std::string, std::vector<std::string> > headers;
			std::string body;
			std::map<std::string, std::vector<std::string> > trailing_headers;
	} t_requestData;

	enum QuoteFlag {
		NO_QUOTE = 0,
		SINGLE_QUOTE = 1,
		DOUBLE_QUOTE = 2

	};

	enum ExpectedBody {
		NO_BODY = 0,
		CHUNKED = 1,
		CONTENT_LENGTH = 2
	};

	enum ChunkedStatus {
		CHUNK_SIZE = 0,
		CHUNK_DATA = 1,
		CHUNK_DATA_END = 2,
		CHUNK_END = 3
	};

	enum headerType {
		HEADER = 0,
		TRAILING = 1
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

			const char* getReadBuffer(void) const;
			uint32_t getReceivedBytes(void) const;
			const uint32_t& getContentLength(void) const;
			const std::string& getRestData(void) const;
			const t_requestData& getRequestData(void) const;
			const RequestStatus& getStatus(void) const;
			const ExpectedBody& getExpectedBody(void) const;
			const ChunkedStatus& getChunkedStatus(void) const;

			void setReadBuffer(const char* buffer);
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
			ChunkedStatus m_chunkedStatus;

			bool parseHead(std::string& input);
			bool parseHeaders(std::string& input, headerType type);
			bool parseBody(std::string& input);

			bool parseHeader(std::string& line, headerType type);
			bool interpretHeaders(headerType type);
			bool parseBodyChunked(std::string& input);
			bool checkHead(const std::vector<std::string>& args);

			// ...
	};

} /* namespace http */
