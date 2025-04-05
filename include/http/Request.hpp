#pragma once

#include "http/AMessage.hpp"
#include "http/Uri.hpp"
#include "http/http.hpp"

namespace http {


	class Request : public AMessage {
		public:
			Request();
			~Request();

			/* Start Line */
			const Method& getMethod() const;
			const Uri& getUri() const;
			Uri& getUri();

			void setMethod(const Method& method);
			void setUri(const Uri& uri);

			/* Debugging */
			std::string toString() const;

		private:
			Method m_method;
			Uri m_uri;
	};

} /* namespace http */
