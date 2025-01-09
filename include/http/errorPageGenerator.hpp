#pragma once

#include "http/constants.hpp"

#include <cstdarg>
#include <iostream>
#include <map>

namespace http {

	std::string generateErrorPage(StatusCode code);

} // namespace http
