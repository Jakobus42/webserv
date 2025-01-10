#pragma once

#include "http/constants.hpp"

namespace http {

	/**
	 * @class Request
	 * @brief ...
	 */
	class Request {
		public:
			Request();
			~Request();
			Request(const Request& other);
			Request& operator=(const Request& rhs);

			std::string toString() const;

			Method getMethod() const;
			const std::string& getUri() const;
			const std::string& getVersion() const;
			const std::string& getBody() const;
			const std::map<std::string, std::string>& getHeaders() const;

			void setMethod(Method method);
			void setUri(const std::string& uri);
			void setVersion(const std::string& version);
			void setBody(const std::string& body);
			void setHeader(const std::string& key, const std::string& value);

		private:
			Method m_method;
			std::string m_uri; // TODO: maybe make uri class
			std::string m_version;

			std::map<std::string, std::string> m_headers;

			std::string m_body;
	};

} /* namespace http */
