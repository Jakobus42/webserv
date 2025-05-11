#pragma once

#include "core/ARequestHandler.hpp"
#include "http/http.hpp"

#include <fstream>

namespace core {

	class PostRequestHandler : public ARequestHandler {
		public:
			PostRequestHandler();
			virtual ~PostRequestHandler();

			virtual bool handle(const http::Request& request, http::Response& response) throw(http::HttpException);
			virtual void reset();

		private:
			virtual void checkPathPermissions(const http::Request&) const throw(http::HttpException);
			std::string generateRandomFileName();
			void createFile(const http::Request& request) throw(http::HttpException);
			void openFile() throw(http::HttpException);
			void writeFile(const std::string& requestBody);

		private:
			static const std::size_t CHUNK_SIZE;
			std::ofstream m_fileStream;
			std::string m_filePath;
			std::size_t m_bytesWritten;
	};

} /* namespace core */
