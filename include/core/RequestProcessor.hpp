#pragma once

#include "shared/NonCopyable.hpp"

namespace http {
	class Request;
	class Response;
}; // namespace http

namespace core {

	class RequestProcessor : shared::mixin::NonCopyable {
		public:
			RequestProcessor();
			~RequestProcessor();

			bool processRequest(http::Request* request);
			http::Response* releaseResponse();

			void reset();

		private:
			http::Response* m_response;
	};

} /* namespace core */
