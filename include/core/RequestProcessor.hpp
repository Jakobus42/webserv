#pragma once

#include "core/CGIProcessor.hpp"
#include "http/http.hpp"
#include "io/Dispatcher.hpp"
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
			explicit RequestProcessor(io::Dispatcher& dispatcher);
			~RequestProcessor();

			bool processRequest(const http::Request& request);
			http::Response* releaseResponse();

			void reset();

		private:
			typedef std::map<http::Method, ARequestHandler*> HandlerMap;

			void generateErrorResponse(http::StatusCode statusCode);

		private:
			CGIProcessor m_cgiProcessor;
			http::Response* m_response;
			HandlerMap m_handlers;
	};

} /* namespace core */
