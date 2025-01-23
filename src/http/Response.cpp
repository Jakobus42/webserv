#include "http/Response.hpp"

namespace http {

	/**
	 * @brief Constructs a new Response object.
	 */
	Response::Response() {
	}

	/**
	 * @brief Destroys the Response object.
	 */
	Response::~Response() {
	}

	shared::Buffer<RESPONSE_BUFFER_SIZE>& Response::getData() { return m_data; }


} /* namespace http */
