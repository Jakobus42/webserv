#include "http2/AMessageParser.hpp"

#include "http/http.hpp"
#include "http/types.hpp"

#include <cstring>
#include <sstream>

namespace http2 {

	const char* AMessageParser::CRLF = "\r\n";

	AMessageParser::AMessageParser()
		: m_message(NULL)
		, m_state(START)
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
				if (m_state == START) {
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
		std::cout << line << std::endl;
	}

} /* namespace http2 */
