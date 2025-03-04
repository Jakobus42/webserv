#pragma once

#include "http/http.hpp"
#include "shared/StringView.hpp"

namespace http2 {

	/**
	 * @class Uri
	 * @brief ...
	 */
	class Uri {
		public:
			Uri();
			~Uri();
			Uri(const Uri& other);
			Uri& operator=(const Uri& rhs);

			const shared::StringView& getScheme() const;
			const shared::StringView& getAuthority() const;
			const shared::StringView& getPath() const;
			in_port_t getPort() const;
			const shared::StringView& getQuery() const;

			void setScheme(const shared::StringView& scheme);
			void setAuthority(const shared::StringView& authority);
			void setPort(in_port_t port);
			void setQuery(const shared::StringView& query);

			/* Debugging */
			std::string toString() const;

		private:
			shared::StringView m_scheme;
			shared::StringView m_authority;
			in_port_t m_port;
			shared::StringView m_path;
			shared::StringView m_query;
	};

} /* namespace http2 */
