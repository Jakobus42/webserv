#pragma once

#include "config/ServerConfig.hpp"
#include "core/CGIProcessor.hpp"
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

	class RouteResult : shared::mixin::NonCopyable {
		public:
			RouteResult(const config::LocationConfig& location, const std::string& filePath)
				: location(location)
				, filePath(filePath) {}

			~RouteResult() {}

		public:
			const config::LocationConfig& location;
			const std::string filePath;
	};

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

			RouteResult* route(const shared::string::StringView& uriPath, const config::LocationConfig& currentLocation, std::size_t depth = 0);

		private:
			const config::ServerConfig m_serverConfig;
			CGIProcessor m_cgiProcessor;
			http::Response* m_response;
			RouteResult* m_route;
			HandlerMap m_handlers;

			static const std::size_t MAX_ROUTE_DEPTH = 8;
	};

} /* namespace core */
