#include "http2/AMessage.hpp"

#include <sstream>

namespace http2 {

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

	void AMessage::setVersion(const shared::StringView& version) { m_version.assign(version.begin(), version.end()); }

	/* Headers */

	const std::vector<std::string>& AMessage::getHeader(const std::string& key) const { return m_headers.at(key); }

	const AMessage::HeaderMap& AMessage::getHeaders() const { return m_headers; }

	void AMessage::appendHeader(const std::string& key, const std::string& value) { m_headers[key].push_back(value); }

	void AMessage::appendHeader(const shared::StringView& key, const shared::StringView& value) { m_headers[std::string(key.begin(), key.end())].push_back(std::string(value.begin(), value.end())); }

	void AMessage::setHeader(const std::string& key, const std::vector<std::string>& values) { m_headers[key] = values; }

	bool AMessage::hasHeader(const std::string& key) const { return m_headers.find(key) != m_headers.end(); }

	/* Body */

	const std::string& AMessage::getBody() const { return m_body; }

	void AMessage::setBody(const std::string& body) { m_body = body; }

	void AMessage::setBody(const shared::StringView& body) { m_body.assign(body.begin(), body.end()); }

	/* Debugging */

	std::string AMessage::toString() const {
		std::ostringstream oss;

		oss << "Headers:\n";
		for (HeaderMap::const_iterator it = m_headers.begin(); it != m_headers.end(); ++it) {
			oss << it->first << ": ";
			for (HeaderMap::value_type::second_type::const_iterator val_it = it->second.begin(); val_it != it->second.end(); ++val_it) {
				oss << *val_it << "; ";
			}
			oss << "\n";
		}
		oss << "\nBody:\n"
			<< m_body;
		return oss.str();
	}

} // namespace http2
