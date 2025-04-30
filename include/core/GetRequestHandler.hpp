#pragma once

#include "core/ARequestHandler.hpp"
#include "http/http.hpp"
#include "shared/Buffer.hpp"

#include <fstream>
#include <vector>

namespace core {

	class GetRequestHandler : public ARequestHandler {
		public:
			GetRequestHandler();
			virtual ~GetRequestHandler();

			virtual bool handle(const http::Request&, http::Response& response);
			virtual void reset();
			void checkFileAccess() const throw(http::HttpException);
			void openFile();
			bool readFile(http::Response& response);

		private:
			static const std::streamsize BUFFER_SIZE = 16384; // 16 KB

			std::vector<char> m_buffer;
			std::ifstream m_fileStream;
			std::streampos m_streamPosition;
	};

} /* namespace core */
