#include "http/ResponseParser.hpp"

#include "http/Response.hpp"
#include "shared/stringUtils.hpp"

namespace http {

	ResponseParserConfig::ResponseParserConfig()
		: messageParserConfig() // 1KB
		, maxReasonPhraseLength(1024) {}

	ResponseParserConfig::ResponseParserConfig(const config::HttpConfig& httpConfig)
		: messageParserConfig(httpConfig)
		, maxReasonPhraseLength(1024) {}

	ResponseParser::ResponseParser(const ResponseParserConfig& conf)
		: AMessageParser(conf.messageParserConfig)
		, m_config(conf)
		, m_response(NULL) {}

	ResponseParser::~ResponseParser() {}

	Response* ResponseParser::releaseResponse() { return static_cast<Response*>(releaseMessage()); }

	void ResponseParser::setConfig(const ResponseParserConfig& config) {
		m_baseConfig = config.messageParserConfig;
		m_config = config;
	}

	AMessage* ResponseParser::createMessage() const { return new Response(); }

	StatusCode ResponseParser::getErrorCode() const { return BAD_GATEWAY; }

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
			throw HttpException(BAD_GATEWAY, "invalid start-line: cant find spaces");
		}

		m_response = static_cast<Response*>(m_message);

		m_response->setVersion(line.substr(0, firstSpace));

		shared::string::StringView codeView = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
		try {
			std::size_t code = shared::string::toNum<std::size_t>(codeView.toString());
			m_response->setStatusCode(numToStatusCode(code));
		} catch (const std::bad_alloc&) {
			throw;
		} catch (const std::exception& e) {
			throw HttpException(BAD_GATEWAY, "invalid status-code: could not parse: " + std::string(e.what()));
		}

		m_response->setReasonPhrase(line.substr(secondSpace + 1));
		if (m_response->getReasonPhrase().size() > m_config.maxReasonPhraseLength) {
			throw HttpException(PAYLOAD_TOO_LARGE, "reason-phrase exceeds size limit");
		}
		return DONE;
	}

	void ResponseParser::interpretHeaders() {
		this->AMessageParser::interpretHeaders();

		m_response = static_cast<Response*>(m_message);

		if (m_response->hasHeader("Status")) {
			const std::string& statusHeader = m_response->getHeader("Status").front();

			std::size_t firstSpace = statusHeader.find(' ');
			std::string codeStr, reasonPhrase;

			if (firstSpace == std::string::npos) {
				codeStr = statusHeader;
			} else {
				codeStr = statusHeader.substr(0, firstSpace);
				reasonPhrase = statusHeader.substr(firstSpace + 1);
			}

			try {
				StatusCode code = numToStatusCode(shared::string::toNum<std::size_t>(codeStr));
				m_response->setStatusCode(code);

				if (!reasonPhrase.empty()) {
					m_response->setReasonPhrase(reasonPhrase);
				} else {
					m_response->setReasonPhrase(statusCodeToMessage(code));
				}
			} catch (const std::bad_alloc&) {
				throw;
			} catch (const std::exception& e) {
				throw HttpException(BAD_GATEWAY, "invalid Status header");
			}

			m_response->removeHeader("Status");
		}
	}

} /* namespace http */
