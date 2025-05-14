#pragma once

#include "config/ServerConfig.hpp"
#include "http/AMessageParser.hpp"
#include "http/http.hpp"

namespace http {

	struct ResponseParserConfig {
			MessageParserConfig messageParserConfig;
			std::size_t maxReasonPhraseLength;
			ResponseParserConfig();
			explicit ResponseParserConfig(const config::ServerConfig& serverConfig);
	};

	class Response;

	class ResponseParser : public AMessageParser {
		public:
			ResponseParser(const ResponseParserConfig& conf = ResponseParserConfig());
			virtual ~ResponseParser();

			Response* releaseResponse();

			void setConfig(const ResponseParserConfig& config);

		private:
			virtual AMessage* createMessage() const;
			virtual StatusCode getErrorCode() const;

			virtual ParseResult parseStartLine();
			virtual void interpretHeaders();

		private:
			ResponseParserConfig m_config;
			Response* m_response;
	};

} /* namespace http */
