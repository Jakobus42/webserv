#include "http/Response.hpp"

#include "shared/stringUtils.hpp"

#include <sstream>

namespace http {

	Response::Response()
		: AMessage()
		, m_reasonPhrase(statusCodeToMessage(m_statusCode)) {
	}

	Response::~Response() {
	}

	const std::string& Response::serialize() {
		if (!m_serializedData.empty()) {
			return m_serializedData;
		}

		m_reasonPhrase = statusCodeToMessage(m_statusCode);
		if (!hasHeader("Content-Length")) {
			appendHeader("Content-Length", shared::string::toString(m_body.size()));
		}

		std::ostringstream ss;

		ss << m_version << " " << m_statusCode << " " << m_reasonPhrase << "\r\n";

		for (HeaderMap::const_iterator it = m_headers.begin(); it != m_headers.end(); ++it) {
			const std::string& headerName = it->first;
			const std::vector<std::string>& headerValues = it->second;

			ss << headerName << ": ";
			for (std::size_t i = 0; i < headerValues.size(); ++i) {
				if (i != 0) {
					ss << ", ";
				}
				ss << headerValues[i];
			}
			ss << "\r\n";
		}

		ss << "\r\n";

		if (!m_body.empty()) {
			ss << m_body;
		}

		m_serializedData = ss.str();
		return m_serializedData;
	}

	const std::string& Response::getReasonPhrase() const { return m_reasonPhrase; }

	void Response::setReasonPhrase(const std::string& phrase) { m_reasonPhrase = phrase; }

	void Response::setReasonPhrase(const shared::string::StringView& phrase) { m_reasonPhrase.assign(phrase.begin(), phrase.size()); }

} /* namespace http */
