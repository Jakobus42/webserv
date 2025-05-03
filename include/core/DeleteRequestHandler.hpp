#pragma once

#include "core/ARequestHandler.hpp"
#include "http/http.hpp"

namespace core {

	class DeleteRequestHandler : public ARequestHandler {
		public:
			DeleteRequestHandler();
			virtual ~DeleteRequestHandler();

			virtual bool handle(const http::Request& request, http::Response& response) throw(http::HttpException);
			virtual void checkPathPermissions(const http::Request& request) const throw(http::HttpException);

			void deleteFile() throw(http::HttpException);
	};

} /* namespace core */
