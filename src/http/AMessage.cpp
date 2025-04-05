#include "http/AMessage.hpp"

#include <sstream>

namespace http {

	bool AMessage::CaseInsensitiveComparator::operator()(const std::string& lhs, const std::string& rhs) const {
		for (std::size_t i = 0, n = std::min(lhs.size(), rhs.size()); i < n; ++i) {
			char l = std::tolower(lhs[i]);
			char r = std::tolower(rhs[i]);
			if (l != r) {
				return l < r;
			}
		}
		return lhs.size() < rhs.size();
	}

	/**
	 * @brief Constructs a new AMessage object.
	 */
	AMessage::AMessage()
		: m_headers()
		, m_body() {}

	/**
	 * @brief Destroys the AMessage object.
	 */
	AMessage::~AMessage() {}

	/* Version */

	const std::string& AMessage::getVersion() const { return m_version; }

	void AMessage::setVersion(const std::string& version) { m_version = version; }

	void AMessage::setVersion(const shared::string::StringView& version) { m_version.assign(version.begin(), version.end()); }

	/* Headers */

	const std::vector<std::string>& AMessage::getHeader(const std::string& key) const { return m_headers.at(key); }

	const AMessage::HeaderMap& AMessage::getHeaders() const { return m_headers; }

	void AMessage::appendHeader(const std::string& key, const std::string& value) { m_headers[key].push_back(value); }

	void AMessage::appendHeader(const shared::string::StringView& key, const shared::string::StringView& value) {
		std::vector<std::string>& target = m_headers[std::string(key.begin(), key.end())];

		target.push_back(std::string());
		target.back().assign(value.begin(), value.end());
	}

	void AMessage::setHeader(const std::string& key, const std::vector<std::string>& values) { m_headers[key] = values; }

	void AMessage::setHeader(const shared::string::StringView& key, const std::vector<shared::string::StringView>& values) {
		std::vector<std::string>& target = m_headers[std::string(key.begin(), key.end())];

		if (target.empty()) {
			target.reserve(values.size());
		}

		for (std::vector<shared::string::StringView>::const_iterator it = values.begin(); it != values.end(); ++it) {
			target.push_back(std::string());
			target.back().assign(it->begin(), it->end());
		}
	}

	bool AMessage::hasHeader(const std::string& key) const { return m_headers.find(key) != m_headers.end(); }

	/* Body */

	const std::string& AMessage::getBody() const { return m_body; }

	void AMessage::setBody(const std::string& body) { m_body = body; }

	void AMessage::setBody(const shared::string::StringView& body) { m_body.assign(body.begin(), body.end()); }

	void AMessage::appendBody(const std::string& body) { m_body.append(body); }

	void AMessage::appendBody(const shared::string::StringView& body) { m_body.append(body.begin(), body.size()); }

	/* Debugging */

	std::string AMessage::toString() const {
		std::ostringstream oss;

		for (HeaderMap::const_iterator it = m_headers.begin(); it != m_headers.end(); ++it) {
			oss << it->first << ": ";
			for (HeaderMap::value_type::second_type::const_iterator val_it = it->second.begin(); val_it != it->second.end(); ++val_it) {
				oss << *val_it << ", ";
			}
			oss << '\n';
		}
		oss << '\n'
			<< m_body;
		return oss.str();
	}

} // namespace http
