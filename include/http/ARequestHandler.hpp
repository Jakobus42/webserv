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

			virtual void handle(const Request& req, Response& res) = 0;

		private:
			config::Location m_locations;
	};

} /* namespace http */
