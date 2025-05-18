#pragma once

#include "config/HttpConfig.hpp"
#include "http/AMessageParser.hpp"
#include "http/http.hpp"

namespace http {

	struct RequestParserConfig {
			MessageParserConfig messageParserConfig;
			std::size_t maxUriLength;
			RequestParserConfig();
			explicit RequestParserConfig(const config::HttpConfig& httpConfig);
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
			void decodeUri(class Uri& uri);
			void parsePath();
			std::string normalizePath(const std::string& path);

			static bool isHexadecimal(char c);
			static char hexToChar(char left, char right);
			static std::string decodeHexEncoded(const std::string& str);

			virtual void interpretHeaders();

		private:
			RequestParserConfig m_config;
			Request* m_request;
	};

} /* namespace http */
