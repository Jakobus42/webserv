#include "http/ResponseParser.hpp"

#include "http/Response.hpp"
#include "shared/stringUtils.hpp"

namespace http {

	ResponseParserConfig::ResponseParserConfig()
		: messageParserConfig() // 1KB
		, maxReasonPhraseLength(1024) {}

	ResponseParserConfig::ResponseParserConfig(const MessageParserConfig& messageParserConfig, std::size_t maxReasonPhraseLength)
		: messageParserConfig(messageParserConfig)
		, maxReasonPhraseLength(maxReasonPhraseLength) {}

	ResponseParser::ResponseParser(const ResponseParserConfig& conf)
		: AMessageParser(conf.messageParserConfig)
		, m_config(conf)
		, m_response(NULL) {}

	ResponseParser::~ResponseParser() {}

	Response* ResponseParser::releaseResponse() { return static_cast<Response*>(releaseMessage()); }

	AMessage* ResponseParser::createMessage() const { return new Response(); }

	/* <HTTP-Version> <Status-Code> <Reason-Phrase> */
	AMessageParser::ParseResult ResponseParser::parseStartLine() {
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

		m_response = static_cast<Response*>(m_message);

		m_response->setVersion(line.substr(0, firstSpace));

		shared::string::StringView codeView = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		try {
			std::size_t code = shared::string::toNum<std::size_t>(codeView.toString());
			m_response->setStatusCode(numToStatusCode(code));
		} catch (const std::exception& e) {
			throw HttpException(BAD_REQUEST, "invalid statuc-code: could not parse: " + std::string(e.what()));
		}

		m_response->setReasonPhrase(line.substr(secondSpace + 1));
		if (m_response->getReasonPhrase().size() > m_config.maxReasonPhraseLength) {
			throw HttpException(PAYLOAD_TOO_LARGE, "reason-phrase exceeds size limit");
		}
		return DONE;
	}

} /* namespace http */
