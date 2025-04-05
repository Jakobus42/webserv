#pragma once

#include "http/AMessage.hpp"
#include "http/Uri.hpp"
#include "http/http.hpp"

namespace http {

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
			const http::Uri& getUri() const;
			http::Uri& getUri();

			void setMethod(const http::Method& method);
			void setUri(const http::Uri& uri);

			/* Debugging */
			std::string toString() const;

		private:
			http::Method m_method;
			http::Uri m_uri;
	};

} /* namespace http */
