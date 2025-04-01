#include "http2/ResponseParser.hpp"

namespace http2 {

	/**
	 * @brief Constructs a new ResponseParser object.
	 */
	ResponseParser::ResponseParser() {}

	/**
	 * @brief Destroys the ResponseParser object.
	 */
	ResponseParser::~ResponseParser() {}

	Response* ResponseParser::releaseResponse() { return static_cast<Response*>(releaseMessage()); }

	AMessage* ResponseParser::createMessage() const { return new Response(); }

	void ResponseParser::parseStartLine() {}

} /* namespace http2 */
