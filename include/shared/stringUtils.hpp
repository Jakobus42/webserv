#pragma once

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace shared {

	namespace string {

		const std::string WHITESPACES = " \t\n\v\f\r";

		void split(const std::string &str, std::vector<std::string> &result, const std::string &delimiters = WHITESPACES);

		void trim(std::string &str, const std::string &delimiters = WHITESPACES);

	} // namespace string

} // namespace shared