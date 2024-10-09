#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace utils {

namespace string {

const std::string WHITESPACES = " \t\n\v\f\r";

int stoi(const std::string& str, size_t* idx = 0, int base = 10);

void split(const std::string& str, std::vector<std::string>& result, const std::string& delimiters = WHITESPACES);

void trim(std::string& str, const std::string& delimiters = WHITESPACES);

// std::vector<std::string> strtok(const std::string& str, const std::string& delimiters);

}  // namespace string

}  // namespace utils