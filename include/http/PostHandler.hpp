#pragma once

#include "http/ARequestHandler.hpp"
#include "http/CGIHandler.hpp"

namespace http {

	class PostHandler : public ARequestHandler {
		public:
			PostHandler();
			virtual ~PostHandler();

			virtual void handle(const Request* request, Response* response);

		private:
			http::CGIHandler m_cgi;
	};

} /* namespace http */
