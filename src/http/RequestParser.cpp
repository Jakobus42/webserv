#include "http/RequestParser.hpp"

#include "http/Request.hpp"
#include "shared/stringUtils.hpp"

// todo: validate host, scheme
// todo: better error messages for uri
// todo: normelize the path here maybe or in the uri class (uri.normelize())
// todo: setPathSegemnt should take a stringView. (see setHeaders for impl example) - also the split path should be implemented here using stringView aswell

namespace http {

	RequestParserConfig::RequestParserConfig()
		: messageParserConfig()
		, maxUriLength(1024) // 1KB
	{}

	RequestParserConfig::RequestParserConfig(const MessageParserConfig& messageParserConfig, std::size_t maxUriLength)
		: messageParserConfig(messageParserConfig)
		, maxUriLength(maxUriLength) {}

	RequestParser::RequestParser(const RequestParserConfig& conf)
		: AMessageParser(conf.messageParserConfig)
		, m_config(conf)
		, m_request(NULL) {}

	RequestParser::~RequestParser() {}

	Request* RequestParser::releaseRequest() { return static_cast<Request*>(releaseMessage()); }

	AMessage* RequestParser::createMessage() const { return new Request(); }

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

		m_request->setVersion(line.substr(secondSpace + 1));

		return DONE;
	}

	void RequestParser::parseUriOriginForm(const shared::string::StringView& uriView) {
		Uri& uri = m_request->getUri();
		if (uriView.find('?') < uriView.find('#')) {
			uri.setQuery(uriView.substr(uriView.find('?') + 1));
		}
		uri.setPath(uriView.substr(0, uriView.find_first_of(shared::string::StringView("?#"))));
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
		parsePath();
	}

	// parse CGI if the path starts with /cgi-bin/
	void RequestParser::parsePath() {
		Uri& uri = m_request->getUri();
		if (uri.getPath().find("/cgi-bin/") == 0) {
			std::size_t pos = uri.getPath().find_first_of("/#?", 9);
			if (pos != std::string::npos) {
				uri.setCgiPathInfo(uri.getPath().substr(pos));
				uri.setPath(uri.getPath().substr(1, pos));
			}
			m_request->setType(Request::CGI);
		}
		uri.setPathSegment(shared::string::split(uri.getPath(), '/'));
		// TODO: should we parse this at all?
		// also, should we check if we accept the script here?
	}


} /* namespace http */
