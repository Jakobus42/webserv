#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class PostHandler
	 * @brief ...
	 */
	class PostHandler : public ARequestHandler {
		public:
			PostHandler(const config::t_location& locations);
			~PostHandler();

			void handle(const Request& req, Response& res);
	};

} /* namespace http */
