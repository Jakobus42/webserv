#pragma once

#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <map>
#include <string>
#include <vector>

namespace http2 {

	/**
	 * @class AMessage
	 * @brief ...
	 */
	class AMessage : shared::NonCopyable {
		public:
			typedef std::map<std::string, std::vector<std::string> > HeaderMap;

			AMessage();
			virtual ~AMessage();

			/* Start Line */
			const std::string& getVersion() const;

			void setVersion(const std::string& version);
			void setVersion(const shared::StringView& version);

			/* Headers */
			const std::vector<std::string>& getHeader(const std::string& key) const;
			const HeaderMap& getHeaders() const;

			void appendHeader(const std::string& key, const std::string& value);
			void appendHeader(const shared::StringView& key, const shared::StringView& value);
			void setHeader(const std::string& key, const std::vector<std::string>& values);

			bool hasHeader(const std::string& key) const;

			/* Body */
			const std::string& getBody() const;

			void setBody(const std::string& body);
			void setBody(const shared::StringView& body);

			/* Debugging */
			std::string toString() const;

		protected:
			HeaderMap m_headers;
			std::string m_body;
			std::string m_version;
	};

} // namespace http2
