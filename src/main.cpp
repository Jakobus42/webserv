#include <algorithm>
#include <iostream>

#include "../include/logger/LoggerManager.hpp"
#include "../include/utils/StringUtils.hpp"

struct PrintElement {
  void operator()(std::string& value) const { std::cout << '"' << value << '"' << std::endl; }
};

int main(void) {
  try {
    std::vector<std::string> result;
    utils::string::split("Hello World!   ", result);
    std::for_each(result.begin(), result.end(), PrintElement());

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
