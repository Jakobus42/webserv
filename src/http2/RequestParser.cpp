#include "http2/RequestParser.hpp"

namespace http2 {

	RequestParserConfig::RequestParserConfig()
		: maxUriLength(1024) // 1KB
		, messageParserConfig() {}

	/**
	 * @brief
	 *
	 */
	RequestParser::RequestParser(const RequestParserConfig& conf)
		: AMessageParser(conf.messageParserConfig) {}

	/**
	 * @brief Destroys the RequestParser object.
	 */
	RequestParser::~RequestParser() {}

	Request* RequestParser::releaseRequest() { return static_cast<Request*>(releaseMessage()); }

	AMessage* RequestParser::createMessage() const { return new Request(); }

	/* <Method> <Request-URI> <HTTP-Version> */
	void RequestParser::parseStartLine() {
		shared::StringView line = readLine();
		if (m_needData) {
			return;
		}

		std::size_t firstSpace = line.find(' ');
		std::size_t secondSpace = line.find(' ', firstSpace + 1);
		if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
			throw http::exception(http::BAD_REQUEST, "invalid start-line: cant find spaces");
		}

		Request* request = static_cast<Request*>(m_message);

		request->setMethod(http::stringToMethod(line.substr(0, firstSpace).to_string()));

		shared::StringView uriView = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		if (uriView.size() > m_config.maxUriLength) {
			throw http::exception(http::PAYLOAD_TOO_LARGE, "uri exceeds size limit");
		}
		parseUri(uriView);

		request->setVersion(line.substr(secondSpace + 1));

		m_state = HEADERS;
	}

	void RequestParser::parseUri(const shared::StringView&) {}


} /* namespace http2 */
