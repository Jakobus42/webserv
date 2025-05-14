#pragma once

#include "core/ARequestHandler.hpp"
#include "http/http.hpp"
#include "shared/fileUtils.hpp"

#include <fstream>
#include <string>
#include <vector>

namespace core {

	class GetRequestHandler : public ARequestHandler {
		public:
			GetRequestHandler();
			virtual ~GetRequestHandler();

			virtual bool handle(const http::Request&, http::Response& response) throw(http::HttpException);
			virtual void reset();

		private:
			virtual void checkPathPermissions(const http::Request& request) const throw(http::HttpException);

			void selectFile();
			void openFile();
			bool readFile(http::Response& response);
			void generateAutoindexResponse(const http::Request& request, http::Response& response);
			std::string generateDirectoryListing(const http::Request& request, const std::string& filePath);
			const std::string& getMimeType() const;

		private:
			static const std::streamsize BUFFER_SIZE = 16384; // 16 KB

			std::vector<char> m_buffer;
			std::ifstream m_fileStream;
			std::streampos m_streamPosition;
			shared::file::FileType m_fileType;
			std::string m_filePath;
			bool m_shouldAutoindex;
	};

} /* namespace core */
