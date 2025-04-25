#pragma once

#include "core/ARequestHandler.hpp"

namespace core {

	class DeleteRequestHandler : public ARequestHandler {
		public:
			DeleteRequestHandler();
			virtual ~DeleteRequestHandler();

			void handle(const http::Request* request, http::Response* response);
	};

} /* namespace core */
