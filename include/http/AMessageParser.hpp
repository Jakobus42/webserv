#pragma once

#include "shared/Buffer.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <vector>

namespace http {

	struct MessageParserConfig {
			std::size_t maxBodySize;
			std::size_t maxHeaderValueLength;
			std::size_t maxHeaderCount;
			std::size_t maxHeaderValueCount;
			std::size_t maxHeaderNameLength;

			MessageParserConfig();
	};

	class AMessage;

	/**
	 * @class AMessageParser
	 * @brief ...
	 */
	class AMessageParser : shared::NonCopyable {
		public:
			static const std::size_t BUFFER_SIZE = 16 * 1024; // 16 KB

			AMessageParser(const MessageParserConfig& conf = MessageParserConfig());
			virtual ~AMessageParser();

			bool parse();

			shared::Buffer<BUFFER_SIZE>& getReadBuffer();

			void reset();

		protected:
			enum ParseState {
				START_LINE,
				HEADERS,
				CHUNK_SIZE,
				CHUNK_BODY,
				BODY,
				TRAILING_HEADERS,
				COMPLETE,
			};

			enum ParseResult {
				DONE,
				CONTINUE,
				NEED_DATA,
			};

			/* Message management */
			AMessage* releaseMessage();

			virtual AMessage* createMessage() const = 0;

			/* Shared */
			std::pair<shared::StringView /*line*/, bool /*ok*/> readLine();
			bool isChunked() const;
			bool isTChar(char c) const;
			bool isVChar(char c) const;

			/* Parsers */
			virtual ParseResult parseStartLine() = 0;

			ParseResult parseHeaderLine();
			void validateHeaders();
			shared::StringView extractHeaderKey(const shared::StringView& line) const;
			std::vector<shared::StringView> extractHeaderValues(const shared::StringView& line) const;

			ParseResult parseChunkSize();

			ParseResult parseBody(bool isChunked);

		protected:
			static const char CRLF[];
			static const char TCHAR[];
			static const char WHITESPACE[];

			MessageParserConfig m_config;
			AMessage* m_message;
			ParseState m_state;
			shared::Buffer<BUFFER_SIZE> m_buffer;
			std::size_t m_contentLength;
	};


} /* namespace http */
