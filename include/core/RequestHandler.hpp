#pragma once

#include "core/AHandler.hpp"

namespace core {

	/**
	 * @class RequestHandler
	 * @brief ...
	 */
	class RequestHandler : public AHandler {
		public:
			RequestHandler(const HandlerContext& ctx);
			~RequestHandler();

			void handle();
			bool shouldDrop() const;

		private:
			RequestHandler();
			RequestHandler(const RequestHandler& other);
			RequestHandler& operator=(const RequestHandler& rhs);
	};

} /* namespace core */
