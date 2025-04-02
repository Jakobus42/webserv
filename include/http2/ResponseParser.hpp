#pragma once

#include "http2/AMessageParser.hpp"
#include "http2/Response.hpp"

namespace http2 {

	/**
	 * @class ResponseParser
	 * @brief ...
	 */
	class ResponseParser : public AMessageParser {
		public:
			ResponseParser(const MessageParserConfig& conf = MessageParserConfig());
			virtual ~ResponseParser();

			Response* releaseResponse();

		private:
			virtual AMessage* createMessage() const;
			virtual void parseStartLine();
	};

} /* namespace http2 */
