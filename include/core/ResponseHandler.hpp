#pragma once

#include "core/IHandler.hpp"

namespace core {

	/**
	 * @class ResponseHandler
	 * @brief ...
	 */
	class ResponseHandler : public IHandler {
		public:
			ResponseHandler();
			~ResponseHandler();

			void handle(HandleContext &ctx);
		private:
			ResponseHandler(const ResponseHandler &other);
			ResponseHandler &operator=(const ResponseHandler &rhs);
	};

} /* namespace core */
