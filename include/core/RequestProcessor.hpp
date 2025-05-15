#pragma once

#include "config/Config.hpp"
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
			explicit RequestProcessor(const config::Config::ServerConfigs& serverConfigs, io::Dispatcher& dispatcher);
			~RequestProcessor();

			void init();
			bool processRequest(const http::Request& request);

			http::Response* releaseResponse();

			void reset();

		private:
			enum State {
				PREPROCESS,
				PROCESS,
				DONE
			};

			typedef std::map<http::Method, ARequestHandler*> HandlerMap;

			void preprocess(const http::Request& request);
			void resolveHost(const http::Request& request);

			bool process(const http::Request& request);
			bool handleFetchRequest(const http::Request& request);
			bool handleCGIRequest(const http::Request& request);
			bool shouldRedirect(const http::Request& request) const;

			void generateErrorResponse(http::StatusCode statusCode);
			void generateRedirectResponse();

		private:
			std::string generateErrorPage(http::StatusCode statusCode);

		private:
			const config::Config::ServerConfigs& m_serverConfigs;

			config::ServerConfig m_serverConfig;
			CGIProcessor m_cgiProcessor;
			http::Response* m_response;
			HandlerMap m_handlers;
			Router m_router;
			State m_state;
	};

} /* namespace core */
