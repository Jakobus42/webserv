#pragma once

#include "http/ARequestHandler.hpp"

namespace http {

	class CGIHandler : public ARequestHandler {
		public:
			CGIHandler();
			~CGIHandler();

			virtual void handle(const Request* request, Response* response);
	};

} /* namespace http */
