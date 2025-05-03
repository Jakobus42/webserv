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

			virtual bool handle(const http::Request&, http::Response& response) throw(http::HttpException);
			virtual void checkPathPermissions(const http::Request& request) const throw(http::HttpException);
			virtual void reset();

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
