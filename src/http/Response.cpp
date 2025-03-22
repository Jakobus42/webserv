#include "http/Response.hpp"

#include "shared/stringUtils.hpp"

#include <ctime>

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: m_statusCode(OK)
		, m_headers()
		, m_body()
		, m_readPos(0)
		, m_data() {
	}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	std::string Response::getDateString() {
		char buffer[64] = "";
		std::time_t now = std::time(NULL);
		std::tm* gmtTime = std::gmtime(&now);
		std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmtTime);
		return std::string(buffer);
	}

	void Response::serialize() {
		std::string statusLine = HTTP_VERSION + " " +
			shared::string::to_string(static_cast<int>(m_statusCode)) + " " +
			getStatusMessage(m_statusCode) + CRLF;
		m_data.append(statusLine.c_str(), statusLine.length());

		setHeader("Date", getDateString());

		std::map<std::string, std::vector<std::string> >::iterator it;
		for (it = m_headers.begin(); it != m_headers.end(); ++it) {
			const std::string& key = it->first;
			const std::vector<std::string>& values = it->second;

			std::vector<std::string>::const_iterator vit;
			for (vit = values.begin(); vit != values.end(); ++vit) {
				std::string headerLine = key + ": " + *vit + CRLF;
				m_data.append(headerLine.c_str(), headerLine.length());
			}
		}
		m_data.append(CRLF.c_str(), CRLF.length());

		m_data.append(m_body.c_str(), m_body.length());
	}

	void Response::setStatusCode(StatusCode statusCode) { m_statusCode = statusCode; }

	void Response::setBody(const std::string& body) { m_body = body; }

	void Response::setHeader(const std::string& key, const std::string& value) { m_headers[key].push_back(value); }

	void Response::appendToBody(const char* data, std::size_t len) { m_body.append(data, len); }

	std::string& Response::getData() { return m_data; }

	StatusCode Response::getStatusCode() const { return m_statusCode; }

	const std::string& Response::getBody() const { return m_body; }

	std::size_t Response::getReadPos() const { return m_readPos; }

	bool Response::eof() const { return m_readPos >= m_data.size(); }

	void Response::advanceReadPos(std::size_t n) {
		m_readPos += n;
		if (m_readPos > m_data.size()) {
			m_readPos = m_data.size();
		}
	}


} /* namespace http */
