#pragma once

#include "http/Request.hpp"
#include "http/Response.hpp"
#include "shared/NonCopyable.hpp"

namespace http {

	/**
	 * @class IRequestHandler
	 * @brief ...
	 */
	class IRequestHandler : shared::NonCopyable {
		public:
			IRequestHandler() {}

			virtual ~IRequestHandler() {}

			virtual void handle(const Request& req, Response& res) = 0;

		private:
	};

} /* namespace http */
