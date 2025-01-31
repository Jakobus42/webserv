#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class PostHandler
	 * @brief ...
	 */
	class PostHandler : public ARequestHandler {
		public:
			PostHandler(Router& router);
			~PostHandler();
			std::string getFilePath(const std::string& path); // TODO: unused

			void handle(const Request& request, Response& response);
	};

} /* namespace http */
