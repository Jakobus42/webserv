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
			std::string getFilePath(const std::string& path);

			void handle(const Request& request, Response& response);
	};

} /* namespace http */
