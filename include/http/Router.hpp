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
		static int getLocation(const std::string& uri, std::vector<config::t_location>& locations, config::t_location* location);
    private:  
		Router();
        ~Router();
        Router(const Router &other);
        Router& operator=(const Router &rhs);

};

} /* namespace http */
