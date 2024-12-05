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

}  // namespace string

}  // namespace shared
