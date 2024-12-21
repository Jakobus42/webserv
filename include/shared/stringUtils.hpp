#pragma once

#include <stdint.h>

#include "shared/defines.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace shared {

	namespace string {

		const std::string WHITESPACES = " \t\n\v\f\r";

		void split(const std::string& str, std::vector<std::string>& result, const std::string& delimiters = WHITESPACES);

		void trim(std::string& str, const std::string& delimiters = WHITESPACES);

		uint32_t posStoi(std::string str, int base = 10);

		uint32_t StoiHex(std::string& str, int& ret);

		bool isGreyspace(char c);
	} // namespace string

} // namespace shared