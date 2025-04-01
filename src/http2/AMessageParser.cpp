#include "http2/AMessageParser.hpp"

#include <cstring>
#include <sstream>

namespace http2 {

	const char* AMessageParser::CRLF = "\r\n";

	AMessageParser::AMessageParser()
		: m_message(NULL) {}

	AMessageParser::~AMessageParser() { delete m_message; }

	std::vector<char>& AMessageParser::getReadBuffer() { return m_buffer; }

	void AMessageParser::parse() {
		if (!m_message) {
			m_message = createMessage();
		}

		while (m_state != COMPLETE) {
			if (m_state == START) {
				this->parseStartLine();
			} else if (m_state == HEADERS) {
				this->parseHeaderLine();
			} else if (m_state == COMPLETE) {
			}
		}
	}

	/* Message management */

	AMessage* AMessageParser::releaseMessage() {
		AMessage* released = m_message;
		m_message = NULL;
		return released;
	}

	/* Shared */

	shared::StringView AMessageParser::readLine() {
		return shared::StringView();
	}

	/*
	   message-header = field-name ":" [ field-value ]
	   field-name     = token
	   field-value    = *( field-content | LWS )
	   field-content  = <the OCTETs making up the field-value
						and consisting of either *TEXT or combinations
						of token, separators, and quoted-string>
	*/
	void AMessageParser::parseHeaderLine() {}

} /* namespace http2 */
