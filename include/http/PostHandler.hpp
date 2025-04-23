#pragma once

#include "http/ARequestHandler.hpp"

namespace http {

	class PostHandler : public ARequestHandler {
		public:
			PostHandler();
			virtual ~PostHandler();

			virtual void handle(const Request* request, Response* response);
	};

} /* namespace http */
