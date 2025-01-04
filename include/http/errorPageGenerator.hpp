#pragma once

#include "shared/defines.hpp"

#include <cstdarg>
#include <iostream>
#include <map>

namespace http {

	std::string generateErrorPage(StatusCode code);

} // namespace http
