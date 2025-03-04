#pragma once

#include "shared/StringView.hpp"

#include <map>
#include <string>
#include <vector>

namespace http2 {

	/**
	 * @class AMessage
	 * @brief ...
	 */
	class AMessage {
		public:
			typedef std::map<shared::StringView, std::vector<shared::StringView> > HeaderMap;

			AMessage();
			virtual ~AMessage();
			AMessage(const AMessage& other);
			AMessage& operator=(const AMessage& rhs);

			/* Start Line */
			const std::string& getVersion() const;

			/* Headers */
			const std::vector<shared::StringView>& getHeader(const shared::StringView& key) const;
			const HeaderMap& getHeaders() const;

			void appendHeader(const shared::StringView& key, const shared::StringView& value);
			void setHeader(const shared::StringView& key, const std::vector<shared::StringView>& values);

			bool hasHeader(const shared::StringView& key) const;

			/* Body */
			const shared::StringView& getBody() const;

			void setBody(const shared::StringView& body);

			/* Debugging */
			std::string toString() const;

		protected:
			static const std::string HTTP_VERSION;

			HeaderMap m_headers;
			shared::StringView m_body;
	};

} // namespace http2
