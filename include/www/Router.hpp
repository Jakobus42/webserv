#pragma once

#include <map>

#include "http/Request.hpp"
#include "string"

namespace www {

// TODO: TMP
enum Method { GET };
class Response {};
//----

/**
 * @brief responsible for handling the http request
 * @param req Contains the Request metadata
 * @param res empty Response to be written to
 */
typedef void (*HandlerFunc)(const http::Request& req, Response& res);

/**
 * @struct Route
 * @brief ...
 */
struct Route {
  std::string relativePath;
  Method method;

  bool operator<(const Route& other) const;
};

/**
 * @class Router
 * @brief ...
 */
class Router {
 public:
  Router();
  ~Router();
  Router(const Router& other);
  Router& operator=(const Router& rhs);

  void registerRoute(const Route& route, const HandlerFunc& handler);
  bool handleRequest(const http::Request& req, Response& res) const;

 private:
  std::map<Route, HandlerFunc> m_routes;
};

} /* namespace www */
