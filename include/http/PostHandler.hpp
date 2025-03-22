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

			void handle(const Request& request, Response& response);

			void reset();

		private:
			void createFile(const Request& request);
			std::string generateFileName() const;

		private:
			std::ofstream m_fileStream;
			size_t m_bytesWritten;
	};

} /* namespace http */
