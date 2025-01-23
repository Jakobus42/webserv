#pragma once

#include "IRequestHandler.hpp"

namespace http {

	/**
	 * @class PostHandler
	 * @brief ...
	 */
	class PostHandler : public IRequestHandler {
		public:
			PostHandler();
			~PostHandler();

			void handle(const Request& req, Response& res);
	};

} /* namespace http */
