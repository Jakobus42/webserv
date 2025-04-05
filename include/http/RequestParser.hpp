#pragma once

#include "http/AMessageParser.hpp"

namespace http {

	struct RequestParserConfig {
			std::size_t maxUriLength;
			MessageParserConfig messageParserConfig;
			RequestParserConfig();
	};

	class Request;

	/**
	 * @class RequestParser
	 * @brief ...
	 */
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
