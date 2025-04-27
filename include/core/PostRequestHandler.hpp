#pragma once

#include "core/ARequestHandler.hpp"

namespace core {

	class PostRequestHandler : public ARequestHandler {
		public:
			PostRequestHandler();
			virtual ~PostRequestHandler();

			virtual void handle(const http::Request& request, http::Response& response);
	};

} /* namespace core */
