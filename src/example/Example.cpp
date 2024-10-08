#include "../../include/example/Example.hpp"

namespace example {

/**
 * @brief Constructs a new Example object.
 */
Example::Example() {}

/**
 * @brief Destroys the Example object.
 */
Example::~Example() {}

/**
 * @brief Copy constructor.
 * @param other The other Example object to copy.
 */
Example::Example(const Example&) {}

/**
 * @brief Copy assignment operator.
 * @param other The other Example object to assign from.
 * @return A reference to the assigned Example object.
 */
Example& Example::operator=(const Example&) { return *this; }

} /* namespace example */
