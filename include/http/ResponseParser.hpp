#pragma once

#include "http/AMessageParser.hpp"

namespace http {

	struct ResponseParserConfig {
			MessageParserConfig messageParserConfig;
			std::size_t maxReasonPhraseLength;
			ResponseParserConfig();
			ResponseParserConfig(const MessageParserConfig& messageParserConfig, std::size_t maxReasonPhraseLength);
	};

	class Response;

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

} /* namespace http */
