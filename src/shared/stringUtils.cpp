#include "shared/stringUtils.hpp"

#include <cstring>

namespace shared {

	namespace string {

		/**
		 * @brief Splits the given string into tokens based on the specified delimiters.
		 *
		 * This function takes a string and splits it into multiple tokens, storing them
		 * in the result vector. Each token is separated by one of the characters in the
		 * delimiters string.
		 *
		 * @param str The string to be split into tokens.
		 * @param delimiters A string containing delimiter characters used to split the
		 * input string.
		 * @param result A vector to store the resulting tokens after splitting the
		 * input string.
		 */
		void split(const std::string& str, std::vector<std::string>& result, const std::string& delimiters) {
			std::size_t start = str.find_first_not_of(delimiters);
			while (start != std::string::npos) {
				std::size_t end = str.find_first_of(delimiters, start);
				result.push_back(str.substr(start, end - start));
				start = str.find_first_not_of(delimiters, end);
			}
		}

		/**
		 * @brief Trims leading and trailing characters from the input string.
		 *
		 * This function removes any characters at the beginning and end of the string
		 * `str` that are found in the `delimiters` string.
		 *
		 * @param str The string to be trimmed. The string is modified in place.
		 * @param delimiters A string containing the characters to remove from the start
		 * and end of `str`.
		 *
		 * @note If the string only contains characters from `delimiters`, it will be
		 * cleared.
		 */
		void trim(std::string& str, const std::string& delimiters) {
			std::size_t start = str.find_first_not_of(delimiters);
			std::size_t end = str.find_last_not_of(delimiters);
			if (start != std::string::npos && end != std::string::npos) {
				str = str.substr(start, end - start + 1);
			} else {
				str.clear();
			}
		}

		std::string toString(int value) {
			std::stringstream ss;
			ss << value;
			return ss.str();
		}

		/**
		 * @brief Converts a hexadecimal string to an unsigned integer.
		 *
		 * This function converts a hexadecimal string to an unsigned integer. The
		 * function will stop converting when it encounters a character that is not a
		 * hexadecimal digit. If the result would exceed the maximum value of a 32-bit
		 * unsigned integer, the function returns 0 and sets the `ret` parameter to -1.
		 *
		 * @param str The hexadecimal string to convert to an integer.
		 * @param ret A reference to an integer that will be set to -1 in case of incorrect input.
		 *
		 * @return The converted integer value, or 0 if the string is not a valid
		 * hexadecimal number. In this case, the `ret` parameter will be set to -1.
		 */
		uint32_t StoiHex(std::string& str, int& ret) {
			uint64_t result = 0;
			uint64_t max = std::numeric_limits<uint32_t>::max();
			size_t i = 0;
			while (i < str.size()) {
				if (str[i] >= '0' && str[i] <= '9') {
					if (result * 16 + str[i] - '0' > max) {
						ret = -1;
						return 0;
					}
					result = result * 16 + str[i] - '0';
				} else if (str[i] >= 'A' && str[i] <= 'F') {
					if (result * 16 + str[i] - 'A' + 10 > max) {
						ret = -1;
						return 0;
					}
					result = result * 16 + str[i] - 'A' + 10;
				} else if (str[i] >= 'a' && str[i] <= 'f') {
					if (result * 16 + str[i] - 'a' + 10 > max) {
						ret = -1;
						return 0;
					}
					result = result * 16 + str[i] - 'a' + 10;
				} else {
					if (i == 0) {
						ret = -1;
						return 0;
					}
					break;
				}
				i++;
			}
			str = str.substr(i);
			return result;
		}

		/**
		 * @brief checks if a character is a whitespace character exept for '\n' and '\r'
		 *
		 * @param c the character to check
		 *
		 * @return a bool indicating if the character is a whitespace character exept for '\n' and '\r'
		 */
		bool isGreyspace(char c) {
			return c == ' ' || c == '\t' || c == '\v' || c == '\f';
		}

		/**
		 * @brief Splits a path into its components.
		 *
		 * This function splits a path into its components, which are separated by
		 * forward slashes. The components are stored in the `result` vector.
		 *
		 * @param path The path to split.
		 * @param result A vector to store the components of the path.
		 *
		 * @return 0 if the path was successfully split, 1 if the path is invalid.
		 */
		int splitPath(const std::string& path, std::vector<std::string>& result) {
			std::vector<std::string> tempResult;
			std::string tmp = path;
			if (tmp[0] != '/') {
				return 1;
			}
			std::stringstream ss(tmp.substr(1));
			std::string segment;
			while (std::getline(ss, segment, '/')) {
				if (!segment.empty()) {
					tempResult.push_back(segment);
				} else {
					return 1;
				}
			}
			result = tempResult;
			return 0;
		}

		std::string fromNum(int32_t number) {
			std::stringstream ss;

			ss << number;
			return ss.str();
		}

	} // namespace string

} // namespace shared
