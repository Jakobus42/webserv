#pragma once

#include "core/AHandler.hpp"

namespace core {

	/**
	 * @class ResponseHandler
	 * @brief ...
	 */
	class ResponseHandler : public AHandler {
		public:
			ResponseHandler();
			~ResponseHandler();

			void handle(HandlerContext& ctx);

		private:
			ResponseHandler(const ResponseHandler& other);
			ResponseHandler& operator=(const ResponseHandler& rhs);
	};

} /* namespace core */
