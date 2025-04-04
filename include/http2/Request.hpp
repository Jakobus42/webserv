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

			/* Start Line */
			const http::Method& getMethod() const;
			const http2::Uri& getUri() const;
			http2::Uri& getUri();

			void setMethod(const http::Method& method);
			void setUri(const http2::Uri& uri);

			/* Debugging */
			std::string toString() const;

		private:
			http::Method m_method;
			http2::Uri m_uri;
	};

} /* namespace http2 */
