#pragma once

#include "config/Parser.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/NonCopyable.hpp"

namespace http {

	/**
	 * @class ARequestHandler
	 * @brief ...
	 */
	class ARequestHandler : shared::NonCopyable {
		public:
			ARequestHandler(const config::Location& locations)
				: m_locations(locations) {}

			virtual ~ARequestHandler() {}

			virtual void handle(const Request& request, Response& response) = 0;

			void handleError(const Request& request, Response& response) {
				response.setBody(getErrorPage(request.getStatusCode()));
				response.setHeader("Content-Length", shared::string::fromNum(response.getBody().length()));
				response.setHeader("Content-Type", TEXT_HTML);
			}

		private:
			config::Location m_locations;
	};

} /* namespace http */
