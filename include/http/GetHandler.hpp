#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class GetHandler
	 * @brief ...
	 */
	class GetHandler : public ARequestHandler {
		public:
			GetHandler(const config::t_location& locations);
			~GetHandler();

			void handle(const Request& req, Response& res);

		private:
	};

} /* namespace http */
