#pragma once

#include "http2/AMessage.hpp"
#include "shared/Buffer2.hpp"
#include "shared/NonCopyable.hpp"

#include <vector>

namespace http2 {

	static const std::size_t BUFFER_SIZE = 16 * 1024; // 16 KB

	/**
	 * @class AMessageParser
	 * @brief ...
	 */
	class AMessageParser : shared::NonCopyable {
		public:
			AMessageParser();
			virtual ~AMessageParser();

			void parse();
			shared::Buffer<BUFFER_SIZE>& getReadBuffer();

		protected:
			enum ParseState {
				START,
				HEADERS,
				BODY,
				COMPLETE,
				ERROR,
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

			/* Parsers */
			virtual void parseStartLine() = 0;
			void parseHeaderLine();

		protected:
			static const char* CRLF;

			AMessage* m_message;
			ParseState m_state;
			shared::Buffer<BUFFER_SIZE> m_buffer;
	};


} /* namespace http2 */
