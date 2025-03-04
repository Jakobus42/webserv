#pragma once

#include "http2/AMessage.hpp"
#include "shared/NonCopyable.hpp"

namespace http2 {

	/**
	 * @class AMessageParser
	 * @brief ...
	 */
	class AMessageParser : shared::NonCopyable {
		public:
			AMessageParser();
			virtual ~AMessageParser();


			AMessage* releaseMessage();

		private:
			virtual void parseStartLine() = 0;

		private:
			AMessage* m_message;
	};


} /* namespace http2 */
