#pragma once

#include "http2/AMessageParser.hpp"

namespace http2 {

	/**
	 * @class RequestParser
	 * @brief ...
	 */
	class RequestParser : public AMessageParser {
		public:
			RequestParser();
			~RequestParser();

		private:
	};

} /* namespace http2 */
