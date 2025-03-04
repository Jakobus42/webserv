#pragma once

#include "shared/NonCopyable.hpp"
#include "shared/Buffer.hpp"

namespace http2 {

	/**
	 * @class MessageParser
	 * @brief ...
	 */
    template<typename T>
	class MessageParser : shared::NonCopyable {
		public:
			MessageParser();
			~MessageParser();
        
        protected:

        private:

	};

} /* namespace http2 */
