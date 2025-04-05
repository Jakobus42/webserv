#pragma once

#include <netinet/in.h>

#include "shared/NonCopyable.hpp"
#include "shared/StringView.hpp"

#include <vector>

// todo: missing getters and setters ...

namespace http {

	class Uri {
		public:
			Uri();
			~Uri();
			Uri(const Uri& other);
			Uri& operator=(const Uri& rhs);

			const std::string& getScheme() const;
			const std::string& getAuthority() const;
			const std::string& getPath() const;
			in_port_t getPort() const;
			const std::string& getQuery() const;

			void setScheme(const std::string& scheme);
			void setScheme(const shared::string::StringView& scheme);
			void setAuthority(const std::string& authority);
			void setAuthority(const shared::string::StringView& authority);
			void setPort(in_port_t port);
			void setPath(const std::string& path);
			void setPath(const shared::string::StringView& path);
			void setQuery(const std::string& query);
			void setQuery(const shared::string::StringView& query);
			void setCgiPathInfo(const std::string& cgiPathInfo);
			void setCgiPathInfo(const shared::string::StringView& cgiPathInfo);
			void setPathSegment(const std::vector<std::string>& pathSegment);

			/* Debugging */
			std::string toString() const;

		private:
			std::string m_scheme;
			std::string m_authority;
			in_port_t m_port;
			std::string m_path;
			std::string m_query;
			std::string m_cgiPathInfo;
			std::vector<std::string> m_pathSegment;
	};

} /* namespace http */
