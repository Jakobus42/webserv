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

			http::Response* processRequest(http::Request* request);

		private:
	};

} /* namespace core */
