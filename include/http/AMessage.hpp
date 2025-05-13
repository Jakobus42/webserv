#pragma once

#include "http/http.hpp"
#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"
#include "shared/stringUtils.hpp"

#include <map>
#include <vector>

namespace http {

	/* HTTP-message   = Request | Response */
	class AMessage : shared::mixin::NonCopyable {
		public:
			typedef std::map<std::string, std::vector<std::string>, shared::string::CaseInsensitiveComparator> HeaderMap;

			AMessage();
			virtual ~AMessage();

			void setIsValid(bool isValid);
			bool isValid() const;

			/* Start Line */
			const std::string& getVersion() const;

			void setVersion(const std::string& version);
			void setVersion(const shared::string::StringView& version);

			StatusCode getStatusCode() const;

			void setStatusCode(StatusCode code);

			/* Headers */
			const std::vector<std::string>& getHeader(const std::string& key) const;
			const HeaderMap& getHeaders() const;

			void appendHeader(const std::string& key, const std::string& value);
			void appendHeader(const shared::string::StringView& key, const shared::string::StringView& value);
			void setHeader(const std::string& key, const std::vector<std::string>& values);
			void setHeader(const shared::string::StringView& key, const std::vector<shared::string::StringView>& values);

			bool hasHeader(const std::string& key) const;

			void removeHeader(const std::string& key);

			/* Body */
			const std::string& getBody() const;

			void setBody(const std::string& body);
			void setBody(const shared::string::StringView& body);
			void appendBody(const std::string& body);
			void appendBody(const shared::string::StringView& body);

			/* Debugging */
			std::string toString() const;

		protected:
			HeaderMap m_headers;
			std::string m_body;
			std::string m_version;

			StatusCode m_statusCode;
			bool m_isValid;
	};

} // namespace http
