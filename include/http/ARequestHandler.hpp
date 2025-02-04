#pragma once

#include "config/Parser.hpp"
#include "http/GoodRouter.hpp"
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
			ARequestHandler(GoodRouter& router)
				: m_router(router) {}

			virtual ~ARequestHandler() {}

			virtual void handle(const Request& request, Response& response) = 0;

			void handleError(Response& response) {
				response.setBody(getErrorPage(response.getStatusCode()));
				response.setHeader("Content-Length", shared::string::fromNum(response.getBody().length()));
				response.setHeader("Content-Type", TEXT_HTML);
			}

		protected:
			GoodRouter& m_router;
	};

} /* namespace http */
