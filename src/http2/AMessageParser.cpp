#include "http2/AMessageParser.hpp"

#include "http/http.hpp"
#include "http/types.hpp"

#include <cstring>
#include <sstream>

namespace http2 {

	MessageParserConfig::MessageParserConfig()
		: maxUriLength(8 * 1024)		 // 8KB
		, maxBodySize(10 * 1024 * 1024)	 // 10MB
		, maxHeaderValueLength(8 * 1024) // 8KB
		, maxHeaderCount(128)			 // 128 headers
		, maxHeaderValueCount(64)		 // 64 values
		, maxHeaderNameLength(256)		 // 256B
	{}

	const char* AMessageParser::CRLF = "\r\n";
	const char AMessageParser::TCHAR[256] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '!', 0, '#', '$', '%', '&', '\'', 0, 0, '*', '+', 0, '-', '.', 0, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, 0, 0, 0, 0, 0, 0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0, 0, 0, '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0, '|', 0, '~', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const char AMessageParser::WHITESPACE[] = " \t";

	/* Public */


	AMessageParser::AMessageParser(const MessageParserConfig& conf = MessageParserConfig())
		: m_config(conf)
		, m_message(NULL)
		, m_state(START_LINE)
		, m_buffer()
		, m_needData(false) {}

	AMessageParser::~AMessageParser() { delete m_message; }

	shared::Buffer<AMessageParser::BUFFER_SIZE>& AMessageParser::getReadBuffer() { return m_buffer; }

	void AMessageParser::parse() {
		try {
			if (!m_message) {
				m_message = createMessage();
			}

			while (m_state != COMPLETE && m_needData != true) {
				if (m_state == START_LINE) {
					this->parseStartLine();
				} else if (m_state == HEADERS) {
					this->parseHeaderLine();
				}
			}
		} catch (const http::exception& e) {
			throw e;
		} catch (const std::exception& e) {
			throw http::exception(http::INTERNAL_SERVER_ERROR, e.what());
		}
	}

	bool AMessageParser::isComplete() const { return m_state == COMPLETE; }

	bool AMessageParser::isPending() const { return m_needData == true; }

	void AMessageParser::reset() {
		m_needData = false;
		m_state = START_LINE;
		delete m_message;
	}

	/* Private */


	/* Message management */

	AMessage* AMessageParser::releaseMessage() {
		AMessage* released = m_message;
		m_message = NULL;
		return released;
	}

	/* Shared */

	shared::StringView AMessageParser::readLine() {
		char* line = m_buffer.readPtr();

		char* lineEnd = std::strstr(line, CRLF);
		if (lineEnd == NULL) {
			m_needData = true;
			return shared::StringView();
		}

		m_buffer.consume(lineEnd + 2 /* CRLF len */ - line);
		return shared::StringView(line, lineEnd - line);
	}

	bool AMessageParser::isTChar(char c) const { return TCHAR[static_cast<unsigned char>(c)] != 0; }

	/* Parsers */

	/*
	   message-header = field-name ":" [ field-value ]
	   field-name     = token
	   field-value    = *( field-content | LWS )
	   field-content  = <the OCTETs making up the field-value
						and consisting of either *TEXT or combinations
						of token, separators, and quoted-string>
	*/
	void AMessageParser::parseHeaderLine() {
		shared::StringView line = readLine();
		if (line.empty()) {
			return;
		}

		if (m_message->getHeaders().size() > m_config.maxHeaderCount) {
			throw http::exception(http::PAYLOAD_TOO_LARGE, "header amount exceeds limit");
		}

		try {
			shared::StringView keyView = extractHeaderKey(line);
			std::vector<shared::StringView> valueViews = extractHeaderValues(line);
			m_message->setHeader(keyView, valueViews);
		} catch (const http::exception& e) {
			throw http::exception(e.getStatusCode(), "invalid header: " + e.getMessage());
		}
	}

	/*
		field-name = token
		token = 1*tchar
	*/
	shared::StringView AMessageParser::extractHeaderKey(const shared::StringView& line) const {
		uint32_t colonPos = line.find(':');
		if (colonPos == shared::StringView::npos) {
			throw http::exception(http::BAD_REQUEST, "missing ':' after field-name");
		}
		if (colonPos == 0) {
			throw http::exception(http::BAD_REQUEST, "empty field-name");
		}
		if (colonPos > m_config.maxHeaderNameLength) {
			throw http::exception(http::PAYLOAD_TOO_LARGE, "header field-name exceeds size limit");
		}

		for (uint32_t i = 0; i < colonPos; ++i) {
			if (!isTChar(line[i])) {
				throw http::exception(http::BAD_REQUEST, std::string("invalid character in field-name '") + line[i] + "'");
			}
		}
		return line.substr(0, colonPos);
	}

	std::vector<shared::StringView> AMessageParser::extractHeaderValues(const shared::StringView&) const {
		return std::vector<shared::StringView>();
	}


} /* namespace http2 */
