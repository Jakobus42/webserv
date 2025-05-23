#include "http/AMessage.hpp"

#include <sstream>

namespace http {

	AMessage::AMessage()
		: m_headers()
		, m_body()
		, m_version("HTTP/1.1")
		, m_statusCode(OK)
		, m_isValid(true) {}

	AMessage::~AMessage() {}

	void AMessage::setIsValid(bool isValid) { m_isValid = isValid; }

	bool AMessage::isValid() const { return m_isValid; }

	/* Version */

	const std::string& AMessage::getVersion() const { return m_version; }

	void AMessage::setVersion(const std::string& version) { m_version = version; }

	void AMessage::setVersion(const shared::string::StringView& version) { m_version.assign(version.begin(), version.end()); }

	http::StatusCode AMessage::getStatusCode() const { return m_statusCode; }

	void AMessage::setStatusCode(StatusCode code) { m_statusCode = code; }

	/* Headers */

	const std::vector<std::string>& AMessage::getHeader(const std::string& key) const { return m_headers.at(key); }

	const AMessage::HeaderMap& AMessage::getHeaders() const { return m_headers; }

	void AMessage::appendHeader(const std::string& key, const std::string& value) { m_headers[key].push_back(value); }

	void AMessage::appendHeader(const shared::string::StringView& key, const shared::string::StringView& value) {
		std::vector<std::string>& target = m_headers[std::string(key.begin(), key.end())];

		target.push_back(std::string());
		target.back().assign(value.begin(), value.end());
	}

	void AMessage::setHeader(const std::string& key, const std::string& value) {
		m_headers[key].clear();
		appendHeader(key, value);
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

	void AMessage::removeHeader(const std::string& key) { m_headers.erase(key); }

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
