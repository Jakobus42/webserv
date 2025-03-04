#include "http2/Request.hpp"

namespace http2 {

/**
 * @brief Constructs a new Request object.
 */
Request::Request() {

}

/**
 * @brief Destroys the Request object.
 */
Request::~Request() {

}

/**
 * @brief Copy constructor.
 * @param other The other Request object to copy.
 */
Request::Request(const Request &other) {

}

/**
 * @brief Copy assignment operator.
 * @param other The other Request object to assign from.
 * @return A reference to the assigned Request object.
 */
Request& Request::operator=(const Request &rhs) {
    return *this;
}

} /* namespace http2 */
