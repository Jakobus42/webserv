#pragma once

#include "ARequestHandler.hpp"

namespace http {

	/**
	 * @class GetHandler
	 * @brief ...
	 */
	class GetHandler : public ARequestHandler {
		public:
			GetHandler(Router& router);
			~GetHandler();

			void handle(const Request& request, Response& response);

		private:
			bool getFilePath(const config::Location& location, const std::string& filePath, std::string& target);
	};

} /* namespace http */
