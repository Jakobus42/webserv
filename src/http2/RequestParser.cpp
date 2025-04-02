#include "http2/RequestParser.hpp"

namespace http2 {

	/**
	 * @brief
	 *
	 */
	RequestParser::RequestParser(const MessageParserConfig& conf)
		: AMessageParser(conf) {}

	/**
	 * @brief Destroys the RequestParser object.
	 */
	RequestParser::~RequestParser() {}

	Request* RequestParser::releaseRequest() { return static_cast<Request*>(releaseMessage()); }

	AMessage* RequestParser::createMessage() const { return new Request(); }

	void RequestParser::parseStartLine() {
		m_state = HEADERS;
	}

} /* namespace http2 */
