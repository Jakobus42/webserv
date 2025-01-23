#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class DeleteHandler
	 * @brief ...
	 */
	class DeleteHandler : public ARequestHandler {
		public:
			DeleteHandler(const config::t_location& locations);
			~DeleteHandler();

			void handle(const Request& req, Response& res);

		private:
	};

} /* namespace http */
