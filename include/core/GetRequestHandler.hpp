#pragma once

#include "core/ARequestHandler.hpp"

namespace core {

	class GetRequestHandler : public ARequestHandler {
		public:
			GetRequestHandler();
			virtual ~GetRequestHandler();

			virtual bool handle(const http::Request& request, http::Response& response);
	};

} /* namespace core */
