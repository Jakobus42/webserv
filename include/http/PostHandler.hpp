#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class PostHandler
	 * @brief ...
	 */
	class PostHandler : public ARequestHandler {
		public:
			PostHandler(const config::Location& locations);
			~PostHandler();

			void handle(const Request& req, Response& res);
	};

} /* namespace http */
