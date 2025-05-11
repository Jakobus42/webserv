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

			void init();
			bool processRequest(const http::Request& request);

			http::Response* releaseResponse();

			void reset();

		private:
			typedef std::map<http::Method, ARequestHandler*> HandlerMap;

			bool handleFetchRequest(const http::Request& request);
			bool handleCGIRequest(const http::Request& request);
			bool shouldRedirect(const http::Request& request) const;
			void generateErrorResponse(http::StatusCode statusCode);
			void generateRedirectResponse();

		private:
			const config::ServerConfig& m_serverConfig;
			CGIProcessor m_cgiProcessor;
			http::Response* m_response;
			HandlerMap m_handlers;
			Router m_router;
	};

} /* namespace core */
