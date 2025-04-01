#include "http2/Uri.hpp"

#include <ostream>

namespace http2 {

	Uri::Uri()
		: m_scheme()
		, m_authority()
		, m_port(0)
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

	void Uri::setScheme(const std::string& scheme) { m_scheme = scheme; }

	void Uri::setAuthority(const std::string& authority) { m_authority = authority; }

	void Uri::setPort(in_port_t port) { m_port = port; }

	void Uri::setQuery(const std::string& query) { m_query = query; }

	std::string Uri::toString() const {
		std::ostringstream oss;

		oss << m_scheme << "://" << m_authority << ":" << m_port << m_path;
		if (!m_query.empty()) {
			oss << "?" << m_query;
		}
		return oss.str();
	}

} /* namespace http2 */
