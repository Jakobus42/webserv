#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class DeleteHandler
	 * @brief ...
	 */
	class DeleteHandler : public ARequestHandler {
		public:
			DeleteHandler(const config::Location& locations);
			~DeleteHandler();

			void handle(const Request& req, Response& res);

		private:
	};

} /* namespace http */
