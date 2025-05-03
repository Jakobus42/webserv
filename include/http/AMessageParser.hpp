#pragma once

#include "shared/Buffer.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <vector>

namespace http {

	struct MessageParserConfig {
			bool strictMode;
			bool requireStartLine;

			std::size_t maxBodySize;
			std::size_t maxHeaderValueLength;
			std::size_t maxHeaderCount;
			std::size_t maxHeaderValueCount;
			std::size_t maxHeaderNameLength;

			MessageParserConfig();
			MessageParserConfig(bool strictMode, std::size_t maxBodySize, std::size_t maxHeaderValueLength, std::size_t maxHeaderCount, std::size_t maxHeaderValueCount, std::size_t maxHeaderNameLength);
	};

	class AMessage;

	class AMessageParser : shared::mixin::NonCopyable {
		public:
			static const std::size_t BUFFER_SIZE = 32 * 1024; // 32 KB

			AMessageParser(const MessageParserConfig& conf = MessageParserConfig());
			virtual ~AMessageParser();

			bool parse();
			bool isComplete() const;

			shared::container::Buffer<BUFFER_SIZE>& getReadBuffer();

			void reset();

			void setConfig(const MessageParserConfig& config);

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
			std::pair<shared::string::StringView /*line*/, bool /*ok*/> readLine();
			bool isChunked() const;
			bool isTChar(char c) const;
			bool isVChar(char c) const;

			/* Parsers */
			virtual ParseResult parseStartLine() = 0;

			ParseResult parseHeaderLine();
			void validateHeaders();
			shared::string::StringView extractHeaderKey(const shared::string::StringView& line) const;
			std::vector<shared::string::StringView> extractHeaderValues(const shared::string::StringView& line) const;

			ParseResult parseChunkSize();

			ParseResult parseBody(bool isChunked);

		protected:
			static const shared::string::StringView CRLF;
			static const shared::string::StringView LF;
			static const char TCHAR[];
			static const shared::string::StringView WHITESPACE;

			MessageParserConfig m_baseConfig;
			AMessage* m_message;
			ParseState m_state;
			shared::container::Buffer<BUFFER_SIZE> m_buffer;
			std::size_t m_contentLength;
	};


} /* namespace http */
