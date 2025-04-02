#pragma once

#include "http2/AMessageParser.hpp"
#include "http2/Request.hpp"

namespace http2 {

	/**
	 * @class RequestParser
	 * @brief ...
	 */
	class RequestParser : public AMessageParser {
		public:
			RequestParser(const MessageParserConfig& conf = MessageParserConfig());
			virtual ~RequestParser();

			Request* releaseRequest();

		private:
			virtual AMessage* createMessage() const;
			virtual void parseStartLine();
	};

} /* namespace http2 */
