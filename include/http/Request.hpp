#pragma once

#include "http/AMessage.hpp"
#include "http/Uri.hpp"
#include "http/http.hpp"

namespace http {

	class Request : public AMessage {
		public:
			enum Type {
				FETCH,
				CGI
			};

			Request();

			~Request();

			/* Start Line */
			const Method& getMethod() const;
			const Uri& getUri() const;
			Uri& getUri();

			void setMethod(const Method& method);
			void setUri(const Uri& uri);

			/* Helpers */
			void setType(Type type);
			Type getType() const;

			/* Debugging */
			std::string toString() const;

		private:
			Method m_method;
			Uri m_uri;

			Type m_type;
	};

} /* namespace http */
