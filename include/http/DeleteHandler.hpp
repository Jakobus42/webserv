#pragma once

#include "IRequestHandler.hpp"

namespace http {

	/**
	 * @class DeleteHandler
	 * @brief ...
	 */
	class DeleteHandler : public IRequestHandler {
		public:
			DeleteHandler();
			~DeleteHandler();

			void handle(const Request& req, Response& res);

		private:
	};

} /* namespace http */
