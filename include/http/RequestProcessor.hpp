#pragma once

#include "config/Parser.hpp"
#include "http/ARequestHandler.hpp"
#include "http/GoodRouter.hpp"
#include "http/RequestParser.hpp"
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
			RequestProcessor(GoodRouter& router);
			~RequestProcessor();

			Response* process(Request& req);

			// TODO: remove testing function
			int testParseURI(const std::string& uri, int mode);

		private:
			Response* releaseResponse();

		private:
			Response* m_res;
			std::map<Method, ARequestHandler*> m_handlers;
			GoodRouter& m_router;
	};

} /* namespace http */
