#pragma once

#include "http/constants.hpp"
#include "http/Response.hpp"

namespace http {

/**
 * @class Router
 * @brief ...
 */
class Router {
    public:
		static void printLocation(const config::t_location location, int detailed);
		static int findLocation(const std::string& uri, std::vector<config::t_location>& locations, config::t_location& location, http::ResponseType& m_type);
    private:
		Router();
        ~Router();
        Router(const Router &other);
        Router& operator=(const Router &rhs);

};

} /* namespace http */
