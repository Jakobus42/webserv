#include "http2/AMessage.hpp"

#include <sstream>

namespace http2 {

	const std::string AMessage::HTTP_VERSION = "HTTP/1.1";

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

	/**
	 * @brief Copy constructor.
	 * @param other The other AMessage object to copy.
	 */
	AMessage::AMessage(const AMessage& other)
		: m_headers(other.m_headers)
		, m_body(other.m_body) {}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other AMessage object to assign from.
	 * @return A reference to the assigned AMessage object.
	 */
	AMessage& AMessage::operator=(const AMessage& rhs) {
		if (this != &rhs) {
			m_headers = rhs.m_headers;
			m_body = rhs.m_body;
		}
		return *this;
	}

	/* Version */
	const std::string& AMessage::getVersion() const { return HTTP_VERSION; }

	/* Headers */

	const std::vector<shared::StringView>& AMessage::getHeader(const shared::StringView& key) const { return m_headers.at(key); }

	const AMessage::HeaderMap& AMessage::getHeaders() const { return m_headers; }

	void AMessage::appendHeader(const shared::StringView& key, const shared::StringView& value) { m_headers.at(key).push_back(value); }

	void AMessage::setHeader(const shared::StringView& key, const std::vector<shared::StringView>& values) { m_headers.insert(std::make_pair(key, values)); }

	bool AMessage::hasHeader(const shared::StringView& key) const { return m_headers.find(key) != m_headers.end(); }

	/* Body */

	const shared::StringView& AMessage::getBody() const { return m_body; }

	void AMessage::setBody(const shared::StringView& body) { m_body = body; }

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
