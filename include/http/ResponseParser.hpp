#pragma once

#include "http/AMessageParser.hpp"

namespace http {

	struct ResponseParserConfig {
			std::size_t maxReasonPhraseLength;
			MessageParserConfig messageParserConfig;
			ResponseParserConfig();
	};

	class Response;

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

} /* namespace http */
