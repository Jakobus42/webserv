#pragma once

#include "http2/AMessageParser.hpp"
#include "http2/Request.hpp"

namespace http2 {

	struct RequestParserConfig {
			std::size_t maxUriLength;
			MessageParserConfig messageParserConfig;
			RequestParserConfig();
	};

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

			virtual void parseStartLine();
			void parseUri(const shared::StringView& uriView);

		private:
			RequestParserConfig m_config;
	};

} /* namespace http2 */
