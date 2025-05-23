#include "http/RequestParser.hpp"

#include "http/Request.hpp"
#include "shared/stringUtils.hpp"

namespace http {

	RequestParserConfig::RequestParserConfig()
		: messageParserConfig()
		, maxUriLength(1024) // 1KB
	{}

	RequestParserConfig::RequestParserConfig(const config::HttpConfig& httpConfig)
		: messageParserConfig(httpConfig)
		, maxUriLength(httpConfig.maxUriLength) {}

	RequestParser::RequestParser(const RequestParserConfig& conf)
		: AMessageParser(conf.messageParserConfig)
		, m_config(conf)
		, m_request(NULL) {}

	RequestParser::~RequestParser() {}

	Request* RequestParser::releaseRequest() { return static_cast<Request*>(releaseMessage()); }

	void RequestParser::setConfig(const RequestParserConfig& config) {
		m_baseConfig = config.messageParserConfig;
		m_config = config;
	}

	AMessage* RequestParser::createMessage() const { return new Request(); }

	StatusCode RequestParser::getErrorCode() const { return BAD_REQUEST; }

	/* <Method> <Request-URI> <HTTP-Version> */
	AMessageParser::ParseResult RequestParser::parseStartLine() {
		std::pair<shared::string::StringView /*line*/, bool /*ok*/> ret = readLine();
		if (ret.second == false) {
			return NEED_DATA;
		}
		shared::string::StringView line = ret.first;

		std::size_t firstSpace = line.find(' ');
		std::size_t secondSpace = line.find(' ', firstSpace + 1);
		if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
			throw HttpException(BAD_REQUEST, "invalid start-line: cant find spaces");
		}

		m_request = static_cast<Request*>(m_message);

		m_request->setMethod(stringToMethod(line.substr(0, firstSpace).toString()));

		shared::string::StringView uriView = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		if (uriView.size() > m_config.maxUriLength) {
			throw HttpException(PAYLOAD_TOO_LARGE, "uri exceeds size limit");
		}

		if (uriView[0] == '/') {
			parseUriOriginForm(uriView);
		} else {
			parseUriAbsoluteForm(uriView);
		}

		shared::string::StringView versionView = line.substr(secondSpace + 1);
		if (versionView != shared::string::StringView("HTTP/1.1")) {
			throw HttpException(HTTP_VERSION_NOT_SUPPORTED, "invalid http version");
		}
		m_request->setVersion(versionView);

		return DONE;
	}

	bool RequestParser::isHexadecimal(char c) {
		if (c <= '9' && c >= '0') {
			return true;
		}
		if (c <= 'f' && c >= 'a') {
			return true;
		}
		if (c <= 'F' && c >= 'A') {
			return true;
		}
		return false;
	}

	char RequestParser::hexToChar(char left, char right) {
		std::stringstream ss;
		unsigned int c = 0;

		ss << std::hex << left << right;
		ss >> c;

		return static_cast<char>(c);
	}

	std::string RequestParser::decodeHexEncoded(const std::string& str) {
		std::string decodedStr;

		for (std::size_t i = 0; i < str.size(); ++i) {
			if (str[i] == '%') {
				if (str.size() - i < 3) {
					throw HttpException(BAD_REQUEST, "Invalid percent-encoded uri");
				}
				char left = str[i + 1];
				char right = str[i + 2];
				if (!isHexadecimal(left) || !isHexadecimal(right)) {
					throw HttpException(BAD_REQUEST, "Invalid percent-encoded uri");
				}
				decodedStr.push_back(hexToChar(left, right));
				i += 2;
			} else {
				decodedStr.push_back(str[i]);
			}
		}
		return decodedStr;
	}

	void RequestParser::decodeUri(Uri& uri) {
		uri.setPath(decodeHexEncoded(uri.getPath()));
		uri.setAuthority(decodeHexEncoded(uri.getAuthority()));
		uri.setQuery(decodeHexEncoded(uri.getQuery()));
		uri.setScheme(decodeHexEncoded(uri.getScheme()));
		uri.setCgiPathInfo(decodeHexEncoded(uri.getCgiPathInfo()));
	}

	void RequestParser::parseUriOriginForm(const shared::string::StringView& uriView) {
		Uri& uri = m_request->getUri();
		if (uriView.find('?') < uriView.find('#')) {
			uri.setQuery(uriView.substr(uriView.find('?') + 1));
		}
		uri.setPath(uriView.substr(0, uriView.find_first_of(shared::string::StringView("?#"))));
		decodeUri(uri);
		parsePath();
	}

	/**
	 * @brief Parses Absolute path and saves only the pure path in "m_truePath" src: (RFC 9112 3.2.2)
	 * @details
	 * absolute-URI = scheme ":" hier-part [ "?" query ]
	 * @param path
	 */
	void RequestParser::parseUriAbsoluteForm(const shared::string::StringView& uriView) {
		Uri& uri = m_request->getUri();
		shared::string::StringView scheme;
		shared::string::StringView authority;
		shared::string::StringView path;

		if (uriView.find(':') == shared::string::StringView::npos) {
			throw HttpException(BAD_REQUEST, "invalid URI");
		}
		scheme = uriView.substr(0, uriView.find(shared::string::StringView(":")));
		if (scheme.empty() || std::isalpha(scheme[0]) == 0 || scheme.find_first_not_of(shared::string::StringView("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-.")) != shared::string::StringView::npos) {
			throw HttpException(BAD_REQUEST, "invalid URI");
		}
		path = uriView.substr(uriView.find(':') + 1, uriView.find('?') - uriView.find(':') - 1);
		if (path[0] != '/' && path[1] != '/') {
			throw HttpException(BAD_REQUEST, "invalid URI");
		}
		path = path.substr(2);
		if (path.find('/') == shared::string::StringView::npos) {
			throw HttpException(BAD_REQUEST, "invalid URI");
		}
		authority = path.substr(0, path.find('/'));
		if (authority.empty()) {
			throw HttpException(BAD_REQUEST, "invalid URI");
		}
		if (uriView.find('?') != shared::string::StringView::npos) {
			uri.setQuery(uriView.substr(uriView.find('?') + 1));
		}
		uri.setPath(path.substr(path.find('/')));
		uri.setScheme(scheme);
		uri.setAuthority(authority);
		decodeUri(uri);
		parsePath();
	}

	// parse CGI if the path starts with /cgi-bin/
	void RequestParser::parsePath() {
		Uri& uri = m_request->getUri();
		uri.setPath(normalizePath(uri.getPath()));
		if (uri.getPath().find("/cgi-bin/") == 0) {
			std::size_t pos = uri.getPath().find_first_of("/#?", 9);
			if (pos != std::string::npos) {
				uri.setCgiPathInfo(uri.getPath().substr(pos));
				uri.setPath(uri.getPath().substr(1, pos - 1));
			} else {
				uri.setPath(uri.getPath().substr(1));
			}
			m_request->setType(Request::CGI);
		}
	}

	std::string RequestParser::normalizePath(const std::string& path) {
		std::vector<std::string> pathSegments = shared::string::split(path, '/');
		std::vector<std::string> normalizedSegments;

		for (std::size_t i = 0; i < pathSegments.size(); ++i) {
			const std::string& segment = pathSegments[i];

			if (segment == ".") {
				continue;
			} else if (segment == "..") {
				if (!normalizedSegments.empty()) {
					normalizedSegments.pop_back();
				}
			} else {
				normalizedSegments.push_back(segment);
			}
		}

		return "/" + shared::string::join(normalizedSegments, "/");
	}

	void RequestParser::interpretHeaders() {
		this->AMessageParser::interpretHeaders();

		m_request = static_cast<Request*>(m_message);

		if (!m_request->hasHeader("host")) {
			throw HttpException(BAD_REQUEST, "host header is missing");
		}

		const std::vector<std::string>& hostValues = m_request->getHeader("host");
		if (hostValues.size() > 1) {
			throw HttpException(BAD_REQUEST, "multiple host headers");
		}

		Uri& uri = m_request->getUri();
		const std::string& host = hostValues.front();

		if (!uri.getAuthority().empty()) {
			if (shared::string::CaseInsensitiveComparator()(host, uri.getAuthority())) {
				throw HttpException(BAD_REQUEST, "host header does not match authority");
			}
		} else {
			uri.setAuthority(host);
		}

		if (m_request->hasHeader("x-filename")) {
			std::vector<std::string> filenameHeader = m_request->getHeader("x-filename");
			if (!filenameHeader.empty()) {
				filenameHeader[0] = normalizePath(filenameHeader[0]);
				m_request->setHeader("x-filename", filenameHeader);
			}
		}
	}


} /* namespace http */
