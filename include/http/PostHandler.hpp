#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class PostHandler
	 * @brief ...
	 */
	class PostHandler : public ARequestHandler {
		public:
			PostHandler(Router& router);
			~PostHandler();

			void handle(const Request& request, Response& response);
	};

} /* namespace http */
