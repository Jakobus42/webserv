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
				COMPLETE = 0x20,
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
			void parseBody();
			void parseChunkSize();
			void parseChunkData();

			bool isTChar(char c) const;

			void setPendingState();
			void clearPendingState();
			void setState(ParseState state);

		private:
			static const char TCHAR[];
			static const char WHITESPACE[];

			static const int PENDING_MASK = 0x1000;

			Request* m_req;

			shared::Buffer<BUFFER_SIZEE> m_buffer;
			std::size_t m_chunkSize;
			std::size_t m_contentLength;
			int m_state;
	};

} /* namespace http */
