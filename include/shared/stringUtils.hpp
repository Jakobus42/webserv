#pragma once

#include <stdint.h>

#include "http/constants.hpp"

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

		template <typename T>
		std::string to_string(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

	} // namespace string

} // namespace shared