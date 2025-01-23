#pragma once

#include "http/IRequestHandler.hpp"
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
			RequestProccesor();
			~RequestProccesor();

			Response* process(const Request& req);


		private:
			std::map<Method, IRequestHandler*> m_handlers;
	};

} /* namespace http */
