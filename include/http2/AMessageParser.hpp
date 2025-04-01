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

			AMessageParser(const MessageParserConfig& conf);
			virtual ~AMessageParser();

			shared::Buffer<BUFFER_SIZE>& getReadBuffer();
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

			struct Token {
					char* begin;
					std::size_t length;
			};

			/* Message management */
			AMessage* releaseMessage();
			virtual AMessage* createMessage() const = 0;

			/* Shared */
			shared::StringView readLine();
			bool isTChar(char c) const;

			/* Parsers */
			virtual void parseStartLine() = 0;
			void parseHeaderLine();

		protected:
			static const char* CRLF;
			static const char TCHAR[];
			static const char WHITESPACE[];

			MessageParserConfig m_config;
			AMessage* m_message;
			ParseState m_state;
			shared::Buffer<BUFFER_SIZE> m_buffer;

			bool m_needData;
	};


} /* namespace http2 */
