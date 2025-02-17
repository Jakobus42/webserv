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

			Response* process(Request& req);

			// TODO: remove testing function
			int testParseURI(const std::string& uri, int mode);

		private:
			Response* releaseResponse();

		private:
			Response* m_res;
			std::map<Method, ARequestHandler*> m_handlers;
			Router& m_router;
	};

} /* namespace http */
