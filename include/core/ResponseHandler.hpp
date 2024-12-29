#pragma once

#include "core/AHandler.hpp"

namespace core {

	/**
	 * @class ResponseHandler
	 * @brief ...
	 */
	class ResponseHandler : public AHandler {
		public:
			ResponseHandler(const HandlerContext& ctx);
			~ResponseHandler();

			void handle();
			bool shouldDrop() const;

		private:
			ResponseHandler();
			ResponseHandler(const ResponseHandler& other);
			ResponseHandler& operator=(const ResponseHandler& rhs);
	};

} /* namespace core */
