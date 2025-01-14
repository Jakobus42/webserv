#include "http/Router.hpp"

namespace http {

/**
 * @brief Constructs a new Router object.
 */
Router::Router() {

}

/**
 * @brief Destroys the Router object.
 */
Router::~Router() {

}

/**
 * @brief Copy constructor.
 * @param other The other Router object to copy.
 */
Router::Router(const Router &other) {
	(void)other;
}

/**
 * @brief Copy assignment operator.
 * @param other The other Router object to assign from.
 * @return A reference to the assigned Router object.
 */
Router& Router::operator=(const Router &rhs) {
    (void)rhs;
	return *this;
}

config::t_location* Router::getLocation(const std::string& uri, const std::vector<config::t_location>& locations) {
	std::cout << "Router: getLocation: " << uri << std::endl;
	(void)locations;
	return NULL;
	for (std::vector<config::t_location>::const_iterator it = locations.begin(); it != locations.end(); ++it) {

	}
}

} /* namespace http */
