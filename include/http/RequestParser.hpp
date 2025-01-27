#pragma once

#include "shared/Buffer.hpp"
#include "shared/NonCopyable.hpp"

#include <http/Request.hpp>
#include <map>

namespace http {

#define REQUEST_BUFFER_SIZE 16 * 1024 // 16KB

	/**
	 * @class RequestParser
	 * @brief ...
	 */
	class RequestParser : shared::NonCopyable {
		public:
			RequestParser();
			~RequestParser();

			void process();
			shared::Buffer<REQUEST_BUFFER_SIZE>& getWriteBuffer();

			void reset();

			bool isComplete() const;
			bool hasError() const;
			bool isPending() const;

			const Request& getRequest();

		private:
			enum ParseState {
				ERROR = 0x0,
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

					Token(const char* token, std::size_t size)
						: token(token)
						, size(size) {}
			};

			char* readLine();

			void parse();
			void parseRequestLine();
			void parseUri();
			std::map<std::string, std::string> parseQuery();
			void parseData();
			void parseChunkSize();
			void parseHeaders();

			// char decodeCharacter(char*& sequence);
			Token extractHeaderKey(char*& line);
			Token extractHeaderValue(char*& line);
			void interpretHeaders();
			void validateContentLength();
			void validateTransferEncoding();

			bool isTChar(char c) const;

			void setPendingState();
			void clearPendingState();
			void setState(ParseState state);

		private:
			static const char TCHAR[];
			static const char WHITESPACE[];
			static const char RESERVED[];

			static const int PENDING_MASK = 0x1000;

			static const std::size_t MAX_URI_LENGTH = 8 * 1024;			 // 8KB
			static const std::size_t MAX_BODY_SIZE = 10 * 1024 * 1024;	 // 10MB
			static const std::size_t MAX_HEADER_VALUE_LENGTH = 8 * 1024; // 8KB
			static const std::size_t MAX_HEADER_COUNT = 128;
			static const std::size_t MAX_HEADER_VALUE_COUNT = 64;
			static const std::size_t MAX_HEADER_NAME_LENGTH = 256;

			Request m_req;

			shared::Buffer<REQUEST_BUFFER_SIZE> m_buffer;
			std::size_t m_contentLength;
			int m_state;
	};

} /* namespace http */
