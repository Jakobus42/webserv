#pragma once

#include "http/ARequestHandler.hpp"

namespace http {

	class GetHandler : public ARequestHandler {
		public:
			GetHandler();
			virtual ~GetHandler();

			virtual void handle(const Request* request, Response* response);
	};

} /* namespace http */
