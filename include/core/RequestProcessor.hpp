#pragma once

#include "http/http.hpp"
#include "shared/NonCopyable.hpp"

#include <map>

namespace http {
	class Request;
	class Response;
} // namespace http

namespace core {

	class ARequestHandler;

	class RequestProcessor : shared::mixin::NonCopyable {
		public:
			RequestProcessor();
			~RequestProcessor();

			bool processRequest(http::Request& request);
			http::Response* releaseResponse();

			void reset();

		private:
			typedef std::map<http::Method, ARequestHandler*> HandlerMap;

			void generateErrorResponse(http::StatusCode statusCode);

		private:
			http::Response* m_response;
			HandlerMap m_handlers;
	};

} /* namespace core */
