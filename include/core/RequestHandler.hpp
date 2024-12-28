#pragma once

#include "core/AHandler.hpp"

namespace core {

	/**
	 * @class RequestHandler
	 * @brief ...
	 */
	class RequestHandler : public AHandler {
		public:
			RequestHandler();
			~RequestHandler();

			void handle(HandlerContext& ctx);
			bool shouldDrop() const;

		private:
			RequestHandler(const RequestHandler& other);
			RequestHandler& operator=(const RequestHandler& rhs);
	};

} /* namespace core */
