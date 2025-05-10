#pragma once

#include "http/AMessageParser.hpp"
#include "http/http.hpp"

namespace http {

	struct RequestParserConfig {
			MessageParserConfig messageParserConfig;
			std::size_t maxUriLength;
			RequestParserConfig();
	};

	class Request;

	class RequestParser : public AMessageParser {
		public:
			RequestParser(const RequestParserConfig& conf = RequestParserConfig());
			virtual ~RequestParser();

			void setConfig(const RequestParserConfig& conf);

			Request* releaseRequest();

		private:
			virtual AMessage* createMessage() const;
			virtual StatusCode getErrorCode() const;

			virtual ParseResult parseStartLine();
			void parseUriOriginForm(const shared::string::StringView& uriView);
			void parseUriAbsoluteForm(const shared::string::StringView& uriView);
			void parsePath();
			std::string normalizePath(const std::string& path);

			virtual void interpretHeaders();

		private:
			RequestParserConfig m_config;
			Request* m_request;
	};

} /* namespace http */
