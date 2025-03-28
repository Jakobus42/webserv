#pragma once

#include "http/ARequestHandler.hpp"
#include "http/RequestParser.hpp"
#include "http/Router.hpp"
#include "http/http.hpp"
#include "shared/NonCopyable.hpp"

#include <deque>
#include <map>

namespace http {

	/**
	 * @class RequestProcessor
	 * @brief ...
	 */
	class RequestProcessor : shared::NonCopyable {
		public:
			RequestProcessor(Router& router);
			~RequestProcessor();

			void process(Request& req);

			int testParseURI(const std::string& uri, int mode);
			void log(const std::string& msg, shared::LogLevel level, const Request& request);

			Response* releaseResponse();
			bool isDone() const;
			void reset();

		private:
			void routeToSafePath(Request& req);
			void handleError(Response* response);

		private:
			Response* m_res;
			std::map<Method, ARequestHandler*> m_handlers;
			Router& m_router;
			bool m_done;
			shared::Logger m_logger;
	};

} /* namespace http */
