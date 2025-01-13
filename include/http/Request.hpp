#pragma once

#include "http/constants.hpp"

#include <vector>

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
			const std::map<std::string, std::vector<std::string> >& getHeaders() const;
			const std::vector<std::string>& getHeader(const std::string& key) const;

			void setMethod(Method method);
			void setUri(const std::string& uri);
			void setVersion(const std::string& version);
			void setBody(const std::string& body);
			void setHeader(const std::string& key, const std::string& value);

			void setMethod(const char* method, std::size_t len);
			void setUri(const char* uri, std::size_t len);
			void setVersion(const char* version, std::size_t len);
			void setBody(const char* body, std::size_t len);
			void setHeader(const char* key, std::size_t keyLen, const char* value, std::size_t valueLen);

			bool hasHeader(const std::string& key) const;
			bool isChunked() const;

			void appendToBody(const std::string& data);
			void appendToBody(const char* data, std::size_t len);

		private:
			void validateUri(const char* uri, std::size_t len);
			void validateVersion(const char* version, std::size_t len);
			void validateHeader(const char* key, std::size_t keyLen, const char* value, std::size_t valueLen);

		private:
			Method m_method;
			std::string m_uri; // TODO: maybe make uri class
			std::string m_version;

			std::map<std::string, std::vector<std::string> > m_headers;

			std::string m_body;
	};

} /* namespace http */
