#include "../../include/utils/StringUtils.hpp"

namespace utils {

namespace string {

int stoi(const std::string&, size_t*, int) {
  // TODO: refer to https://en.cppreference.com/w/cpp/string/basic_string/stol
  return 0;
}

/**
 * @brief Splits the given string into tokens based on the specified delimiters.
 *
 * This function takes a string and splits it into multiple tokens, storing them
 * in the result vector. Each token is separated by one of the characters in the
 * delimiters string.
 *
 * @param str The string to be split into tokens.
 * @param delimiters A string containing delimiter characters used to split the input string.
 * @param result A vector to store the resulting tokens after splitting the input string.
 */
void split(const std::string& str, std::vector<std::string>& result, const std::string& delimiters) {
  std::size_t start = str.find_first_not_of(delimiters);

  while (start != std::string::npos) {
    std::size_t end = str.find_first_of(delimiters, start);
    result.push_back(str.substr(start, end - start));
    start = str.find_first_not_of(delimiters, end);
  }
}

void trim(std::string&, const std::string&) {}

}  // namespace string

}  // namespace utils
