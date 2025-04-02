#pragma once

#include "http2/AMessage.hpp"
#include "shared/Buffer2.hpp"
#include "shared/NonCopyable.hpp"

#include <vector>

namespace http2 {

	struct MessageParserConfig {
			uint32_t maxUriLength;
			uint32_t maxBodySize;
			uint32_t maxHeaderValueLength;
			uint32_t maxHeaderCount;
			uint32_t maxHeaderValueCount;
			uint32_t maxHeaderNameLength;

			MessageParserConfig();
	};

	/**
	 * @class AMessageParser
	 * @brief ...
	 */
	class AMessageParser : shared::NonCopyable {
		public:
			static const uint32_t BUFFER_SIZE = 16 * 1024; // 16 KB

			AMessageParser(const MessageParserConfig& conf = MessageParserConfig());
			virtual ~AMessageParser();

			shared::Buffer2<BUFFER_SIZE>& getReadBuffer();
			void parse();

			bool isComplete() const;
			bool isPending() const;

			void reset();

		protected:
			enum ParseState {
				START_LINE,
				HEADERS,
				BODY,
				COMPLETE,
			};

			/* Message management */
			AMessage* releaseMessage();
			virtual AMessage* createMessage() const = 0;

			/* Shared */
			shared::StringView readLine();
			bool isTChar(char c) const;
			bool isVChar(char c) const;

			/* Parsers */
			virtual void parseStartLine() = 0;

			void parseHeaderLine();
			shared::StringView extractHeaderKey(const shared::StringView& line) const;
			std::vector<shared::StringView> extractHeaderValues(const shared::StringView& line) const;


		protected:
			static const char CRLF[];
			static const char TCHAR[];
			static const char WHITESPACE[];

			MessageParserConfig m_config;
			AMessage* m_message;
			ParseState m_state;
			shared::Buffer2<BUFFER_SIZE> m_buffer;

			bool m_needData;
	};


} /* namespace http2 */
