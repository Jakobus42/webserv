#include "www/Router.hpp"
#include "logger/LoggerManager.hpp"

namespace www {

bool Route::operator<(const Route& other) const {
    if (method != other.method) {
        return method < other.method;
    }
    return relativePath < other.relativePath;
}

Route::Route() : relativePath("/"), method(GET) {}

Route::Route(const std::string& relativePath, Method method) : relativePath(relativePath), method(method) {}



/**
 * @brief Constructs a new Router object.
 */
Router::Router(){}

/**
 * @brief Destroys the Router object.
 */
Router::~Router(){}

/**
 * @brief Copy constructor.
 * @param other The other Router object to copy.
 */
 Router::Router(const Router& other) : m_routes(other.m_routes) {
 }

/**
 * @brief Copy assignment operator.
 * @param other The other Router object to assign from.
 * @return A reference to the assigned Router object.
 */
Router& Router::operator=(const Router& rhs) {
    if (this == &rhs) return *this;
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
