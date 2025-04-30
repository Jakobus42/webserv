#pragma once

#include "config/ServerConfig.hpp"
#include "core/CGIProcessor.hpp"
#include "core/Router.hpp"
#include "http/http.hpp"
#include "io/Dispatcher.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <map>

namespace http {
	class Request;
	class Response;
} // namespace http

namespace core {

	class ARequestHandler;

	class RequestProcessor : shared::mixin::NonCopyable {
		public:
			explicit RequestProcessor(const config::ServerConfig& serverConfig, io::Dispatcher& dispatcher);
			~RequestProcessor();

			bool processRequest(const http::Request& request);
			http::Response* releaseResponse();

			void reset();

		private:
			typedef std::map<http::Method, ARequestHandler*> HandlerMap;

			void generateErrorResponse(http::StatusCode statusCode);
			void generateRedirectResponse();

		private:
			const config::ServerConfig& m_serverConfig;
			CGIProcessor m_cgiProcessor;
			http::Response* m_response;
			HandlerMap m_handlers;
			Router m_router;

			static const std::size_t MAX_ROUTE_DEPTH = 8;
	};

} /* namespace core */
