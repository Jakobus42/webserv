#pragma once

#include "config/Location.hpp"
#include "http/http.hpp"
#include "http/types.hpp"
#include "shared/NonCopyable.hpp"

#include <map>
#include <string>
#include <vector>

namespace http {

	/**
	 * @class Request
	 * @brief ...
	 */
	class Request : shared::NonCopyable {
		public:
			Request();
			~Request();

			std::string toString() const;
			RequestType getType() const;
			Method getMethod() const;
			Uri& getUri();
			const Uri& getUri() const;
			std::string& getUriRaw();
			const std::string& getUriRaw() const;
			const std::string& getVersion() const;
			const std::string& getBody() const;
			const std::map<std::string, std::vector<std::string> >& getHeaders() const;
			const std::vector<std::string>& getHeader(const std::string& key) const;
			StatusCode getStatusCode() const;
			bool hasError() const;
			const config::Location* getLocation() const;

			void setType(RequestType type);
			void setMethod(const char* method, std::size_t len);
			void setUriRaw(const char* uri, std::size_t len);
			void setVersion(const char* version, std::size_t len);
			void setBody(const char* body, std::size_t len);
			void setHeader(const char* key, std::size_t keyLen, const char* value, std::size_t valueLen);
			void setStatusCode(StatusCode statusCode);
			void setLocation(const config::Location* location);

			bool hasHeader(const std::string& key) const;

			void appendToBody(const char* data, std::size_t len);

			void printRequestData() const;

		private:
			void validateUriRaw(const char* uri, std::size_t len);
			void validateVersion(const char* version, std::size_t len);
			void validateHeader(const char* key, std::size_t keyLen, const char* value, std::size_t valueLen);

		private:
			RequestType m_type;
			Method m_method;
			std::string m_uriRaw;
			std::string m_version;

			std::map<std::string, std::vector<std::string> > m_headers;

			std::string m_body;

			http::StatusCode m_statusCode;
			Uri m_uri;
			const config::Location* m_requestedLocation;
	};

} /* namespace http */
