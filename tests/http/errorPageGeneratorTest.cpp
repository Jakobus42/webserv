
#include <gtest/gtest.h>

#include "http/ErrorPageGenerator.hpp"

namespace http {

class ErrorPageGeneratorTest : public ::testing::Test {};

TEST_F(ErrorPageGeneratorTest, CorrectUsage) {
	std::string error_page = generateErrorPage(404);
	ASSERT_NE(error_page, "");
}

TEST_F(ErrorPageGeneratorTest, IncorrectNumber) {
	std::string error_page = generateErrorPage(600);
	ASSERT_EQ(error_page, "");
}

TEST_F(ErrorPageGeneratorTest, NegativeNumber) {
	std::string error_page = generateErrorPage(-1);
	ASSERT_EQ(error_page, "");
}

TEST_F(ErrorPageGeneratorTest, Zero) {
	std::string error_page = generateErrorPage(0);
	ASSERT_EQ(error_page, "");
}

}  // namespace http
