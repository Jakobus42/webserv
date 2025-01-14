#include "http/Request.hpp"

#include "http/http.hpp"

#include <algorithm>
#include <cstring>

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

		std::map<std::string, std::vector<std::string> >::const_iterator it;
		for (it = m_headers.begin(); it != m_headers.end(); ++it) {
			oss << it->first << ": ";
			std::vector<std::string>::const_iterator innerIt;
			for (innerIt = it->second.begin(); innerIt != it->second.end(); ++innerIt) {
				oss << *innerIt;
				if (innerIt + 1 != it->second.end()) {
					oss << ", ";
				}
			}
			oss << CRLF;
		}

		if (!m_headers.empty()) {
			oss << CRLF;
		}

		if (!m_body.empty()) {
			oss << m_body;
		}

		return oss.str();
	}

	bool Request::hasHeader(const std::string& key) const {
		return m_headers.find(key) != m_headers.end();
	}

	bool Request::isChunked() const {
		if (this->hasHeader("transfer-encoding")) {
			const std::vector<std::string>& values = m_headers.at("transfer-encoding");
			return std::find(values.begin(), values.end(), "chunked") != values.end();
		}
		return false;
	}

	void Request::appendToBody(const std::string& data) {
		m_body.append(data);
	}

	void Request::appendToBody(const char* data, std::size_t len) {
		m_body.append(data, len);
	}

	Method Request::getMethod() const { return m_method; }

	const std::string& Request::getUri() const { return m_uri; }

	const std::string& Request::getVersion() const { return m_version; }

	const std::string& Request::getBody() const { return m_body; }

	const std::map<std::string, std::vector<std::string> >& Request::getHeaders() const { return m_headers; }

	const std::vector<std::string>& Request::getHeader(const std::string& key) const { return m_headers.at(key); }

	void Request::setMethod(Method method) { m_method = method; }

	void Request::setMethod(const char* method, std::size_t len) {
		m_method = stringToMethod(std::string(method, len));
	}

	void Request::setUri(const std::string& uri) {
		this->validateUri(uri.c_str(), uri.length());
		m_uri = uri;
	}

	void Request::setUri(const char* uri, std::size_t len) {
		this->validateUri(uri, len);
		m_uri.assign(uri, len);
	}

	void Request::setVersion(const std::string& version) {
		this->validateVersion(version.c_str(), version.length());
		m_version = version;
	}

	void Request::setVersion(const char* version, std::size_t len) {
		this->validateVersion(version, len);
		m_version.assign(version, len);
	}

	void Request::setBody(const std::string& body) { m_body = body; }

	void Request::setBody(const char* body, std::size_t len) { m_body.assign(body, len); }

	void Request::setHeader(const std::string& key, const std::string& value) {
		this->validateHeader(key.c_str(), key.length(), value.c_str(), value.length());
		m_headers[key].push_back(value);
	}

	void Request::setHeader(const char* key, std::size_t keyLen, const char* value, std::size_t valueLen) {
		this->validateHeader(key, keyLen, value, valueLen);
		m_headers[std::string(key, keyLen)].push_back(std::string(value, valueLen));
	}

	void Request::validateUri(const char* uri, std::size_t len) {
		if (len == 0 || uri == NULL) {
			throw http::exception(BAD_REQUEST, "URI can not be empty");
		}
		if (uri[0] != '/') {
			throw http::exception(BAD_REQUEST, "URI must start with '/'");
		}
	}

	void Request::validateVersion(const char* version, std::size_t len) {
		if (version == NULL || len == 0 || std::strcmp(version, HTTP_VERSION.c_str())) {
			throw http::exception(NOT_IMPLEMENTED, "unsupported HTTP version: " + std::string(version, len));
		}
	}

	void Request::validateHeader(const char* key, std::size_t keyLen, const char*, std::size_t) {
		if (key == NULL || keyLen == 0) {
			throw http::exception(BAD_REQUEST, "header key can not be empty");
		}
	}



} /* namespace http */
