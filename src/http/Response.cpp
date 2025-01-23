#include "http/Response.hpp"

#include "shared/stringUtils.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response()
		: m_code(OK)
		, m_headers()
		, m_body()
		, m_data() {
	}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	void Response::serialize() {
		m_data.reset();

		std::string statusLine = HTTP_VERSION + " " +
			shared::string::to_string(static_cast<int>(m_code)) + " " +
			getStatusMessage(m_code) + CRLF;
		m_data.append(statusLine.c_str(), statusLine.length());

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

	void Response::setCode(StatusCode code) { m_code = code; }

	void Response::setBody(const std::string& body) { m_body = body; }

	void Response::setHeader(const std::string& key, const std::string& value) { m_headers[key].push_back(value); }

	shared::Buffer<RESPONSE_BUFFER_SIZE>& Response::getData() { return m_data; }


} /* namespace http */
