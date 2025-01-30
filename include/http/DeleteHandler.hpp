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

			void handle(const Request& request, Response& response);

		private:
	};

} /* namespace http */
