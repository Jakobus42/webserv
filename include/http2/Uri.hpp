#pragma once

#include "http/http.hpp"
#include "shared/NonCopyable.hpp"

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

			const std::string& getScheme() const;
			const std::string& getAuthority() const;
			const std::string& getPath() const;
			in_port_t getPort() const;
			const std::string& getQuery() const;

			void setScheme(const std::string& scheme);
			void setAuthority(const std::string& authority);
			void setPort(in_port_t port);
			void setQuery(const std::string& query);

			/* Debugging */
			std::string toString() const;

		private:
			std::string m_scheme;
			std::string m_authority;
			in_port_t m_port;
			std::string m_path;
			std::string m_query;
	};

} /* namespace http2 */
