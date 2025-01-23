#pragma once

#include "IRequestHandler.hpp"

namespace http {

	/**
	 * @class GetHandler
	 * @brief ...
	 */
	class GetHandler : public IRequestHandler {
		public:
			GetHandler();
			~GetHandler();

			void handle(const Request& req, Response& res);

		private:
	};

} /* namespace http */
