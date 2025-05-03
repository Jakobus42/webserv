#pragma once

#include "core/ARequestHandler.hpp"
#include "http/http.hpp"

namespace core {

	class PostRequestHandler : public ARequestHandler {
		public:
			PostRequestHandler();
			virtual ~PostRequestHandler();

			virtual bool handle(const http::Request& request, http::Response& response);
			virtual void checkPathPermissions() const throw(http::HttpException);
	};

} /* namespace core */
