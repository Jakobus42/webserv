#pragma once

#include "http/http.hpp"
#include "http2/AMessage.hpp"

namespace http2 {

	/**
	 * @class Response
	 * @brief ...
	 */
	class Response : public AMessage {
		public:
			Response();
			~Response();
			Response(const Response& other);
			Response& operator=(const Response& rhs);

		private:
			http::StatusCode m_statusCode;
	};

} /* namespace http2 */
