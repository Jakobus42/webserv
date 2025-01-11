#pragma once

#include <http/Request.hpp>
#include <vector>

namespace http {

	const std::size_t BUFFER_SIZEEEE = 1024;

	enum ParseState {
		STATE_START,
		STATE_HEADERS,
		STATE_BODY,
		STATE_CHUNK_SIZE,
		STATE_CHUNK_DATA,
		STATE_COMPLETE
	};

	/**
	 * @class RequestParser
	 * @brief ...
	 */
	class RequestParser {
		public:
			RequestParser();
			~RequestParser();

			void feed(const char* data, std::size_t size);

			void reset();

			bool isComplete() const;

			const Request& getRequest() const;

		private:
			RequestParser(const RequestParser&);
			RequestParser& operator=(const RequestParser&);

			void compactBuffer();

			char* findCRLF();
			const char* readLine();

			void parse();
			void parseRequestLine();
			void parseHeaders();
			void parseBody();
			void parseChunkSize();
			void parseChunkData();

		private:
			Request m_req; // for now (keep copying in mind)

			char m_buffer[BUFFER_SIZEEEE];
			std::size_t m_readPos;
			std::size_t m_writePos;

			ParseState m_state;
	};

} /* namespace http */
