#pragma once

#include "http2/AMessage.hpp"
#include "shared/Buffer2.hpp"
#include "shared/NonCopyable.hpp"

#include <vector>

namespace http2 {

	struct MessageParserConfig {
			std::size_t maxUriLength;
			std::size_t maxBodySize;
			std::size_t maxHeaderValueLength;
			std::size_t maxHeaderCount;
			std::size_t maxHeaderValueCount;
			std::size_t maxHeaderNameLength;

			MessageParserConfig();
	};

	/**
	 * @class AMessageParser
	 * @brief ...
	 */
	class AMessageParser : shared::NonCopyable {
		public:
			static const std::size_t BUFFER_SIZE = 16 * 1024; // 16 KB

			AMessageParser(const MessageParserConfig& conf = MessageParserConfig());
			virtual ~AMessageParser();

			void parse();
			
			shared::Buffer2<BUFFER_SIZE>& getReadBuffer();
			bool isComplete() const;
			bool isPending() const;

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

			/* Message management */
			AMessage* releaseMessage();
			virtual AMessage* createMessage() const = 0;

			/* Shared */
			shared::StringView readLine();
			bool isChunked() const;
			bool isTChar(char c) const;
			bool isVChar(char c) const;

			/* Parsers */
			virtual void parseStartLine() = 0;

			void parseHeaderLine();
			void validateHeaders();
			shared::StringView extractHeaderKey(const shared::StringView& line) const;
			std::vector<shared::StringView> extractHeaderValues(const shared::StringView& line) const;

			void parseChunkSize();

			void parseBody();


		protected:
			static const char CRLF[];
			static const char TCHAR[];
			static const char WHITESPACE[];

			MessageParserConfig m_config;
			AMessage* m_message;
			ParseState m_state;
			shared::Buffer2<BUFFER_SIZE> m_buffer;
			std::size_t m_contentLength;
			bool m_needData;
	};


} /* namespace http2 */
