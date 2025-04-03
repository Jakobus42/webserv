#include "http2/ResponseParser.hpp"

#include "http/http.hpp"
#include "shared/stringUtils.hpp"

namespace http2 {

	ResponseParserConfig::ResponseParserConfig()
		: maxReasonPhraseLength(1024) // 1KB
		, messageParserConfig() {}

	/**
	 * @brief Constructs a new ResponseParser object.
	 */
	ResponseParser::ResponseParser(const ResponseParserConfig& conf)
		: AMessageParser(conf.messageParserConfig) {}

	/**
	 * @brief Destroys the ResponseParser object.
	 */
	ResponseParser::~ResponseParser() {}

	Response* ResponseParser::releaseResponse() { return static_cast<Response*>(releaseMessage()); }

	AMessage* ResponseParser::createMessage() const { return new Response(); }

	/* <HTTP-Version> <Status-Code> <Reason-Phrase> */
	void ResponseParser::parseStartLine() {
		shared::StringView line = readLine();
		if (m_needData) {
			return;
		}

		std::size_t firstSpace = line.find(' ');
		std::size_t secondSpace = line.find(' ', firstSpace + 1);
		if (firstSpace == std::string::npos || secondSpace == std::string::npos) {
			throw http::exception(http::BAD_REQUEST, "invalid start-line: cant find spaces");
		}

		Response* response = static_cast<Response*>(m_message);

		response->setVersion(line.substr(0, firstSpace));

		shared::StringView codeView = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		std::size_t code = shared::string::toNum<std::size_t>(codeView.to_string(), 10);
		response->setStatusCode(http::toStatusCode(code));

		response->setReasonPhrase(line.substr(secondSpace + 1));
		if (response->getReasonPhrase().size() > m_config.maxReasonPhraseLength) {
			throw http::exception(http::PAYLOAD_TOO_LARGE, "reason-phrase exceeds size limit");
		}

		m_state = HEADERS;
	}

} /* namespace http2 */
