#pragma once

#include "http2/AMessageParser.hpp"
#include "http2/Response.hpp"

namespace http2 {

	struct ResponseParserConfig {
			std::size_t maxReasonPhraseLength;
			MessageParserConfig messageParserConfig;
			ResponseParserConfig();
	};

	/**
	 * @class ResponseParser
	 * @brief ...
	 */
	class ResponseParser : public AMessageParser {
		public:
			ResponseParser(const ResponseParserConfig& conf = ResponseParserConfig());
			virtual ~ResponseParser();

			Response* releaseResponse();

		private:
			virtual AMessage* createMessage() const;

			virtual ParseResult parseStartLine();

		private:
			ResponseParserConfig m_config;
			Response* m_response;
	};

} /* namespace http2 */
