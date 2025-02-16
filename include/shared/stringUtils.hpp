#pragma once

#include <stdint.h>

#include "http/http.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace shared {

	namespace string {

		const std::string WHITESPACES = " \t\n\v\f\r";

		void split(const std::string& str, std::vector<std::string>& result, const std::string& delimiters = WHITESPACES);

		void trim(std::string& str, const std::string& delimiters = WHITESPACES);

		uint32_t StoiHex(std::string& str, int& ret);

		template <typename T>
		T toNum(const std::string& str, int base) {
			T result;
			std::istringstream stream(str);

			if (base == 16) {
				stream >> std::hex;
			} else if (base == 8) {
				stream >> std::oct;
			}
			stream >> result;
			if (stream.fail() || !stream.eof())
				throw std::runtime_error("cant convert to number");
			return result;
		}

		bool isGreyspace(char c);
		int splitPath(const std::string& path, std::vector<std::string>& result);
		std::string fromNum(int32_t number);

		template <typename T>
		std::string to_string(const T& value) {
			std::ostringstream oss;
			oss << value;
			return oss.str();
		}

	} // namespace string

} // namespace shared