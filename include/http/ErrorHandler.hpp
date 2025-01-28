#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class ErrorHandler
	 * @brief ...
	 */
	class ErrorHandler : public ARequestHandler {
		public:
			ErrorHandler(const config::Location& locations);
			~ErrorHandler();

			void handle(const Request& req, Response& res);

		private:
	};

} /* namespace http */
