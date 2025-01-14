#pragma once

#include "http/constants.hpp"
#include "config/Parser.hpp"

namespace http {

/**
 * @class Router
 * @brief ...
 */
class Router {
    public:
		static config::t_location* getLocation(const std::string& uri, const std::vector<config::t_location>& locations);
    private:  
		Router();
        ~Router();
        Router(const Router &other);
        Router& operator=(const Router &rhs);

};

} /* namespace http */
