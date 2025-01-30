#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class GetHandler
	 * @brief ...
	 */
	class GetHandler : public ARequestHandler {
		public:
			GetHandler(const config::Location& locations);
			~GetHandler();

			void handle(const Request& request, Response& response);

		private:
	};

} /* namespace http */
