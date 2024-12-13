#pragma once

#include "http/ErrorMessages.hpp"

#include <string>

namespace http {

	const std::string createResponse(StatusCode status_code);
	const std::string status(StatusCode status_code);

} // namespace http