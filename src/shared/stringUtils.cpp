#include "shared/stringUtils.hpp"

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

		/**
		 * @brief Converts a string to an unsigned integer.
		 *
		 * This function converts a string to an unsigned integer using the specified
		 * base. The function will stop converting when it encounters a character that
		 * is not a digit in the given base. If the result would exceed the maximum
		 * value of a 32-bit unsigned integer, the function returns -1.
		 *
		 * @param str The string to convert to an integer.
		 * @param base The base to use for the conversion. The default is 10.
		 *
		 * @return The converted integer value, or -1 if the result would exceed the
		 * maximum value of a 32-bit unsigned integer.
		 */
		uint32_t posStoi(std::string str, int base) {
			uint32_t result = 0;
			uint64_t max = std::numeric_limits<uint32_t>::max();
			size_t i = 0;
			while (i < str.size() && str[i] >= '0' && str[i] <= '9') {
				if (result * base + str[i] - '0' > max) {
					return -1;
				}
				result = result * base + str[i] - '0';
				i++;
			}
			if (i != str.size()) {
				return -1;
			}
			return result;
		}

	} // namespace string

} // namespace shared
