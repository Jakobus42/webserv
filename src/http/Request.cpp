#include "http/Request.hpp"

#include "http/http.hpp"

#include <algorithm>
#include <cstring>

namespace http {

	/**
	 * @brief Constructs a new Request object.
	 */
	Request::Request()
		: m_type(FETCH)
		, m_method(GET)
		, m_uriRaw("")
		, m_version(HTTP_VERSION)
		, m_statusCode(OK)
		, m_uri()
		, m_requestedLocation(NULL)
		, m_fileType(FILE) {
	}

	/**
	 * @brief Destroys the Request object.
	 */
	Request::~Request() {
	}

	std::string Request::toString() const {
		std::ostringstream oss;

		oss << getMethodString(m_method) << " "
			<< m_uriRaw << " "
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

	bool Request::needsSafePath() const {
		return m_uri.safeAbsolutePath.empty();
	}

	void Request::appendToBody(const char* data, std::size_t len) {
		m_body.append(data, len);
	}

	RequestType Request::getType() const { return m_type; }

	StatusCode Request::getStatusCode() const { return m_statusCode; }

	Method Request::getMethod() const { return m_method; }

	Uri& Request::getUri() { return m_uri; }

	const Uri& Request::getUri() const { return m_uri; }

	std::string& Request::getUriRaw() { return m_uriRaw; }

	const std::string& Request::getUriRaw() const { return m_uriRaw; }

	const std::string& Request::getVersion() const { return m_version; }

	const std::string& Request::getBody() const { return m_body; }

	const std::map<std::string, std::vector<std::string> >& Request::getHeaders() const { return m_headers; }

	const std::vector<std::string>& Request::getHeader(const std::string& key) const { return m_headers.at(key); }

	const config::Location* Request::getLocation() const { return m_requestedLocation; }

	FileType Request::getFileType() const { return m_fileType; }

	bool Request::hasError() const {
		return m_statusCode >= 400;
	}

	void Request::setType(RequestType type) { m_type = type; }

	void Request::setMethod(const char* method, std::size_t len) {
		m_method = stringToMethod(std::string(method, len));
	}

	void Request::setUriRaw(const char* uri, std::size_t len) {
		this->validateUriRaw(uri, len);
		m_uriRaw.assign(uri, len);
	}

	void Request::setVersion(const char* version, std::size_t len) {
		this->validateVersion(version, len);
		m_version.assign(version, len);
	}

	void Request::setBody(const char* body, std::size_t len) { m_body.assign(body, len); }

	void Request::setHeader(const char* key, std::size_t keyLen, const char* value, std::size_t valueLen) {
		this->validateHeader(key, keyLen, value, valueLen);
		m_headers[std::string(key, keyLen)].push_back(std::string(value, valueLen));
	}

	void Request::setStatusCode(StatusCode statusCode) { m_statusCode = statusCode; }

	void Request::setLocation(const config::Location* location) { m_requestedLocation = location; }

	void Request::setFileType(FileType type) { m_fileType = type; }

	void Request::validateUriRaw(const char* uri, std::size_t len) {
		if (len == 0 || uri == NULL) {
			throw http::exception(BAD_REQUEST, "URI can not be empty");
		}
		if (uri[0] != '/') {
			throw http::exception(BAD_REQUEST, "URI must start with '/'");
		}
	}

	void Request::validateVersion(const char* version, std::size_t len) {
		if (version == NULL || len == 0) {
			throw http::exception(BAD_REQUEST, "malformed HTTP version: " + std::string(version, len));
		}
		if (std::strcmp(version, HTTP_VERSION.c_str())) {
			throw http::exception(HTTP_VERSION_NOT_SUPPORTED, "unsupported HTTP version: " + std::string(version, len));
		}
	}

	void Request::validateHeader(const char* key, std::size_t keyLen, const char*, std::size_t) {
		if (key == NULL || keyLen == 0) {
			throw http::exception(BAD_REQUEST, "header key can not be empty");
		}
	}

	void Request::printRequestData() const {
		std::cout << "Method: " << getMethodString(m_method) << std::endl;
		std::cout << "URI: " << m_uriRaw << std::endl;
		std::cout << "Version: " << m_version << std::endl;
		std::cout << "Headers: " << std::endl;
		std::map<std::string, std::vector<std::string> >::const_iterator it;
		for (it = m_headers.begin(); it != m_headers.end(); ++it) {
			std::cout << it->first << ": ";
			std::vector<std::string>::const_iterator innerIt;
			for (innerIt = it->second.begin(); innerIt != it->second.end(); ++innerIt) {
				std::cout << *innerIt;
				if (innerIt + 1 != it->second.end()) {
					std::cout << ", ";
				}
			}
			std::cout << std::endl;
		}
		std::cout << "Body: " << m_body << std::endl;
	}



} /* namespace http */
