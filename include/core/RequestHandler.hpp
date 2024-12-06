#pragma once

#include "core/IHandler.hpp"

namespace core {

	/**
	 * @class RequestHandler
	 * @brief ...
	 */
	class RequestHandler : public IHandler {
		public:
			RequestHandler();
			~RequestHandler();

			void handle(HandleContext &ctx);
		private:
			RequestHandler(const RequestHandler &other);
			RequestHandler &operator=(const RequestHandler &rhs);
	};

} /* namespace core */
