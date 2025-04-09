#pragma once

#include "http/ARequestHandler.hpp"

namespace http {

	class DeleteHandler : public ARequestHandler {
		public:
			DeleteHandler();
			virtual ~DeleteHandler();

			void handle(const Request* request, Response* response);
	};

} /* namespace http */
