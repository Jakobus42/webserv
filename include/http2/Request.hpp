#pragma once

#include "http/http.hpp"
#include "http2/AMessage.hpp"
#include "http2/Uri.hpp"

namespace http2 {

	/**
	 * @class Request
	 * @brief ...
	 */
	class Request : public AMessage {
		public:
			Request();
			~Request();
			Request(const Request& other);
			Request& operator=(const Request& rhs);

			/* Start Line */
			const http::Method& getMethod() const;
			const http2::Uri& getUri() const;

			void setMethod(const http::Method& method);
			void setUri(const http2::Uri& uri);

			/* Debugging */
			std::string toString() const;

		private:
			http::Method m_method;
			http2::Uri m_uri;
	};

} /* namespace http2 */
