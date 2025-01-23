#pragma once

#include "config/Parser.hpp"
#include "http/ARequestHandler.hpp"
#include "http/http.hpp"
#include "shared/NonCopyable.hpp"

#include <deque>
#include <http/RequestParser.hpp>
#include <map>

namespace http {

	/**
	 * @class RequestProccesor
	 * @brief ...
	 */
	class RequestProccesor : shared::NonCopyable {
		public:
			RequestProccesor(const config::t_location& locations);
			~RequestProccesor();

			Response* process(const Request& req);

		private:
			Response* releaseResponse();

		private:
			std::map<Method, ARequestHandler*> m_handlers;
			Response* m_res;
			const config::t_location& m_locations;
	};

} /* namespace http */
