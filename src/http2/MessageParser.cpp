#include "http2/MessageParser.hpp"

#include "http2/Request.hpp"
#include "http2/Response.hpp"

namespace http2 {

	/**
	 * @brief Constructs a new MessageParser object.
	 */
	template <typename T>
	MessageParser<T>::MessageParser() {}

	/**
	 * @brief Destroys the MessageParser object.
	 */
	template <typename T>
	MessageParser<T>::~MessageParser() {}

} /* namespace http2 */
