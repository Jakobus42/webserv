#include "http/Request.hpp"

#include "http/constants.hpp"

namespace http {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: m_method(GET)
		, m_uri("/")
		, m_version(HTTP_VERSION) {
	}

	/**
	 * @brief Destroys the Request object.
	 */
	Request::~Request() {
	}

	/**
	 * @brief Copy constructor.
	 * @param other The other Request object to copy.
	 */
	Request::Request(const Request& other)
		: m_method(other.m_method)
		, m_uri(other.m_uri)
		, m_version(other.m_version)
		, m_headers(other.m_headers)
		, m_body(other.m_body) {
	}

	/**
	 * @brief Copy assignment operator.
	 * @param other The other Request object to assign from.
	 * @return A reference to the assigned Request object.
	 */
	Request& Request::operator=(const Request& rhs) {
		if (this != &rhs) {
			m_method = rhs.m_method;
			m_uri = rhs.m_uri;
			m_version = rhs.m_version;
			m_body = rhs.m_body;
			m_headers = rhs.m_headers;
		}
		return *this;
	}

	std::string Request::toString() const {
		std::ostringstream oss;

		oss << getMethodString(m_method) << " "
			<< m_uri << " "
			<< m_version << CRLF;

		std::map<std::string, std::string>::const_iterator it;
		for (it = m_headers.begin(); it != m_headers.end(); ++it) {
			oss << it->first << ": " << it->second << CRLF;
		}

		if (!m_headers.empty()) {
			oss << CRLF;
		}

		if (!m_body.empty()) {
			oss << m_body;
		}

		return oss.str();
	}

	Method Request::getMethod() const { return m_method; }

	const std::string& Request::getUri() const { return m_uri; }

	const std::string& Request::getVersion() const { return m_version; }

	const std::string& Request::getBody() const { return m_body; }

	const std::map<std::string, std::string>& Request::getHeaders() const { return m_headers; }

	void Request::setMethod(Method method) { m_method = method; }

	void Request::setUri(const std::string& uri) {
		if (uri.empty()) {
			throw std::invalid_argument("URI cannot be empty");
		}
		if (uri[0] != '/') {
			throw std::invalid_argument("URI must start with '/'");
		}

		m_uri = uri;
	}

	void Request::setVersion(const std::string& version) {
		if (version != HTTP_VERSION) {
			throw std::invalid_argument("unsupported HTTP version: " + version);
		}
		m_version = version;
	}

	void Request::setBody(const std::string& body) { m_body = body; }

	void Request::setHeader(const std::string& key, const std::string& value) {
		if (key.empty()) {
			throw std::invalid_argument("header key cannot be empty");
		}
		m_headers[key] = value;
	}

} /* namespace http */
