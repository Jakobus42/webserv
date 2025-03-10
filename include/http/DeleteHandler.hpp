#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class DeleteHandler
	 * @brief ...
	 */
	class DeleteHandler : public ARequestHandler {
		public:
			DeleteHandler(Router& router);
			~DeleteHandler();

			void handle(const Request& request, Response& response);
	};

} /* namespace http */
