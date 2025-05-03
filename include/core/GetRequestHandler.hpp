#pragma once

#include "core/ARequestHandler.hpp"
#include "http/http.hpp"

#include <fstream>
#include <string>
#include <vector>

namespace core {

	class GetRequestHandler : public ARequestHandler {
		public:
			GetRequestHandler();
			virtual ~GetRequestHandler();

			virtual bool handle(const http::Request&, http::Response& response);
			virtual void reset();
			virtual void checkPathPermissions() const throw(http::HttpException);
			void openFile();
			bool readFile(http::Response& response);
			bool shouldAutoindex() const;
			void generateAutoindexResponse(const http::Request& request, http::Response& response);
			std::string generateDirectoryListing(const http::Request& request, const std::string& filePath);

		private:
			static const std::streamsize BUFFER_SIZE = 16384; // 16 KB

			std::vector<char> m_buffer;
			std::ifstream m_fileStream;
			std::streampos m_streamPosition;
	};

} /* namespace core */
