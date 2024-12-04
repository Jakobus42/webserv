#include "www/Router.hpp"

#include "logger/LoggerManager.hpp"

namespace www {

bool Route::operator<(const Route&) const {
  LOG("not implemented yet", logger::DEBUG)
  return true;
}

/**
 * @brief Constructs a new Router object.
 */
Router::Router(){LOG("not implemented yet", logger::DEBUG)}

/**
 * @brief Destroys the Router object.
 */
Router::~Router(){LOG("not implemented yet", logger::DEBUG)}

/**
 * @brief Copy constructor.
 * @param other The other Router object to copy.
 */
Router::Router(const Router&){LOG("not implemented yet", logger::DEBUG)}

/**
 * @brief Copy assignment operator.
 * @param other The other Router object to assign from.
 * @return A reference to the assigned Router object.
 */
Router& Router::operator=(const Router&) {
  LOG("not implemented yet", logger::DEBUG)
  return *this;
}

void Router::registerRoute(const Route& route, const HandlerFunc& handler) {
  m_routes.insert(std::make_pair(route, handler));
}
bool Router::handleRequest(const http::Request&, Response&) const {
  LOG("not implemented yet", logger::DEBUG)
  return true;
}

} /* namespace www */
