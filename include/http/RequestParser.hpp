#pragma once

#include "http/AMessageParser.hpp"

namespace http {

	struct RequestParserConfig {
			MessageParserConfig messageParserConfig;
			std::size_t maxUriLength;
			RequestParserConfig();

			RequestParserConfig(MessageParserConfig messageParserConfig, std::size_t maxUriLength);
	};

	class Request;

	class RequestParser : public AMessageParser {
		public:
			RequestParser(const RequestParserConfig& conf = RequestParserConfig());
			virtual ~RequestParser();

			Request* releaseRequest();

		private:
			virtual AMessage* createMessage() const;

			virtual ParseResult parseStartLine();
			void parseUriOriginForm(const shared::string::StringView& uriView);
			void parseUriAbsoluteForm(const shared::string::StringView& uriView);
			void parsePath();

		private:
			RequestParserConfig m_config;
			Request* m_request;
	};

} /* namespace http */
