#pragma once

#include "http/http.hpp"
#include "shared/NonCopyable.hpp"

#include <map>

namespace http {
	class Request;
	class Response;
	class ARequestHandler;
} // namespace http

namespace core {


	class RequestProcessor : shared::mixin::NonCopyable {
		public:
			RequestProcessor();
			~RequestProcessor();

			bool processRequest(http::Request* request);
			http::Response* releaseResponse();

			void reset();

		private:
			void generateErrorResponse(http::StatusCode statusCode);

		private:
			http::Response* m_response;
			std::map<http::Method, http::ARequestHandler*> m_handlers;
	};

} /* namespace core */
