#pragma once

#include "ARequestHandler.hpp"

#include <fstream>

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
			void reset();

		private:
			void getFilePath(const config::Location& location, const std::string& filePath, std::string& target);
			std::ifstream m_fileStream;
	};

} /* namespace http */
