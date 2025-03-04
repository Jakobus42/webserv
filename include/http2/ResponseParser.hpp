#pragma once

#include "http2/AMessageParser.hpp"

namespace http2 {

	/**
	 * @class ResponseParser
	 * @brief ...
	 */
	class ResponseParser : public AMessageParser {
		public:
			ResponseParser();
			~ResponseParser();

            void parseStartLine();

		private:
	};

} /* namespace http2 */
