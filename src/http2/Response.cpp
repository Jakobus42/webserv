#include "http2/Response.hpp"

namespace http2 {

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

/**
 * @brief Copy constructor.
 * @param other The other Response object to copy.
 */
Response::Response(const Response &other) {

}

/**
 * @brief Copy assignment operator.
 * @param other The other Response object to assign from.
 * @return A reference to the assigned Response object.
 */
Response& Response::operator=(const Response &rhs) {
    return *this;
}

} /* namespace http2 */
