#include "www/Router.hpp"

namespace www {

bool Route::operator<(const Route& other) const {
  if (method != other.method) {
    return method < other.method;
  }
  return relativePath < other.relativePath;
}

Route::Route() : relativePath("/"), method(http::GET) {}

Route::Route(const std::string& relativePath, http::Method method) : relativePath(relativePath), method(method) {}

/**
 * @brief Constructs a new Router object.
 */
Router::Router() {}

/**
 * @brief Destroys the Router object.
 */
Router::~Router() {}

/**
 * @brief Copy constructor.
 * @param other The other Router object to copy.
 */
Router::Router(const Router& other) : m_routes(other.m_routes) {}

/**
 * @brief Copy assignment operator.
 * @param other The other Router object to assign from.
 * @return A reference to the assigned Router object.
 */
Router& Router::operator=(const Router& rhs) {
  if (this != &rhs) {
    m_routes = rhs.m_routes;
  }
  return *this;
}

void Router::registerRoute(const Route& route, const HandlerFunc& handler) {
  m_routes.insert(std::make_pair(route, handler));
}

void Router::processRequest(const http::Request& req, Response& res) const {
  std::map<Route, HandlerFunc>::const_iterator it;

  it = m_routes.find(Route(req.getUri(), req.getMethod()));
  if (it == m_routes.end()) {
    // TODO: write http not found response
    // create response class and Head class for http headers
    return;
  }
  it->second(req, res);
}

} /* namespace www */
