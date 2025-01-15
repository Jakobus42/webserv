#pragma once

#include "shared/Buffer.hpp"

#include <http/Request.hpp>

namespace http {

#define BUFFER_SIZEE 16 * 1024 // 16KB

	/**
	 * @class RequestParser
	 * @brief ...
	 */
	class RequestParser {
		public:
			RequestParser();
			~RequestParser();

			void process();
			shared::Buffer<BUFFER_SIZEE>& getWriteBuffer();

			void reset();

			bool isComplete() const;
			bool isPending() const;

			Request* releaseRequest();

		private:
			RequestParser(const RequestParser&);
			RequestParser& operator=(const RequestParser&);

			enum ParseState {
				START = 0x01,
				HEADERS = 0x02,
				BODY = 0x04,
				CHUNK_SIZE = 0x08,
				CHUNK_DATA = 0x10,
				TRAILING_HEADERS = 0x20,
				COMPLETE = 0x40,
			};

			struct Token {
					const char* token;
					std::size_t size;

					Token()
						: token(NULL)
						, size(0) {}

					Token(const char* token, std::size_t size)
						: token(token)
						, size(size) {}
			};

			char* readLine();

			void parse();
			void parseRequestLine();

			void parseHeaders();
			Token extractHeaderKey(char*& line);
			Token extractHeaderValue(char*& line);
			void interpretHeaders();
			void validateContentLength();
			void validateTransferEncoding();

			void parseData();
			void parseChunkSize();

			bool isTChar(char c) const;

			void setPendingState();
			void clearPendingState();
			void setState(ParseState state);

		private:
			static const char TCHAR[];
			static const char WHITESPACE[];

			static const int PENDING_MASK = 0x1000;

			static const size_t MAX_URI_LENGTH = 8 * 1024; // 8KB
			static const size_t MAX_HEADER_COUNT = 128;
			static const size_t MAX_HEADER_VALUE_COUNT = 64;
			static const size_t MAX_HEADER_NAME_LENGTH = 256;		// 256B
			static const size_t MAX_HEADER_VALUE_LENGTH = 8 * 1024; // 8KB
			static const size_t MAX_BODY_SIZE = 10 * 1024 * 1024;	// 10MB

			Request* m_req;

			shared::Buffer<BUFFER_SIZEE> m_buffer;
			std::size_t m_contentLength;
			int m_state;
	};

} /* namespace http */
