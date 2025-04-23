#include "http/Uri.hpp"

#include <sstream>

namespace http {

	Uri::Uri()
		: m_scheme()
		, m_authority()
		, m_port(8080)
		, m_path()
		, m_query() {}

	Uri::~Uri() {}

	Uri::Uri(const Uri& other)
		: m_scheme(other.m_scheme)
		, m_authority(other.m_authority)
		, m_port(other.m_port)
		, m_path(other.m_path)
		, m_query(other.m_query) {}

	Uri& Uri::operator=(const Uri& rhs) {
		if (this != &rhs) {
			m_scheme = rhs.m_scheme;
			m_authority = rhs.m_authority;
			m_port = rhs.m_port;
			m_query = rhs.m_query;
		}
		return *this;
	}

	const std::string& Uri::getScheme() const { return m_scheme; }

	const std::string& Uri::getAuthority() const { return m_authority; }

	const std::string& Uri::getPath() const { return m_path; }

	in_port_t Uri::getPort() const { return m_port; }

	const std::string& Uri::getQuery() const { return m_query; }

	const std::string& Uri::getCgiPathInfo() const { return m_cgiPathInfo; }

	void Uri::setScheme(const std::string& scheme) { m_scheme = scheme; }

	void Uri::setScheme(const shared::string::StringView& scheme) { m_scheme.assign(scheme.begin(), scheme.size()); }

	void Uri::setAuthority(const std::string& authority) { m_authority = authority; }

	void Uri::setAuthority(const shared::string::StringView& authority) { m_authority.assign(authority.begin(), authority.size()); }

	void Uri::setPort(in_port_t port) { m_port = port; }

	void Uri::setPath(const std::string& path) { m_path = path; }

	void Uri::setPath(const shared::string::StringView& path) { m_path.assign(path.begin(), path.size()); }

	void Uri::setQuery(const std::string& query) { m_query = query; }

	void Uri::setQuery(const shared::string::StringView& query) { m_query.assign(query.begin(), query.size()); }

	void Uri::setCgiPathInfo(const std::string& cgiPathInfo) { m_cgiPathInfo = cgiPathInfo; }

	void Uri::setCgiPathInfo(const shared::string::StringView& cgiPathInfo) { m_cgiPathInfo.assign(cgiPathInfo.begin(), cgiPathInfo.size()); }

	void Uri::setPathSegment(const std::vector<std::string>& pathSegment) { m_pathSegment = pathSegment; }

	std::string Uri::toString() const {
		std::ostringstream oss;

		oss << m_scheme << "://" << m_authority << ":" << m_port << m_path;
		if (!m_query.empty()) {
			oss << "?" << m_query;
		}
		return oss.str();
	}

} /* namespace http */
