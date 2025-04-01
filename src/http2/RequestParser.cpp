#include "http2/RequestParser.hpp"

namespace http2 {

	/**
	 * @brief Constructs a new RequestParser object.
	 */
	RequestParser::RequestParser() {
	}

	/**
	 * @brief Destroys the RequestParser object.
	 */
	RequestParser::~RequestParser() {
	}

	Request* RequestParser::releaseRequest() { return static_cast<Request*>(releaseMessage()); }

	AMessage* RequestParser::createMessage() const { return new Request(); }

	void RequestParser::parseStartLine() {
		m_state = HEADERS;
	}

} /* namespace http2 */
