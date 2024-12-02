
#include <gtest/gtest.h>
#include "../../include/error_page/Generator.hpp"

namespace error_page {

class GeneratorTest : public ::testing::Test {
	
};

TEST_F(GeneratorTest, CorrectUsage) {
	
	std::string error_page = generateErrorPage(404);
	ASSERT_NE(error_page, "");
}

TEST_F(GeneratorTest, IncorrectNumber) {
	
	std::string error_page = generateErrorPage(600);
	ASSERT_EQ(error_page, "");
}

TEST_F(GeneratorTest, NegativeNumber) {
	
	std::string error_page = generateErrorPage(-1);
	ASSERT_EQ(error_page, "");
}

TEST_F(GeneratorTest, Zero) {
	
	std::string error_page = generateErrorPage(0);
	ASSERT_EQ(error_page, "");
}

} /* namespace error_page */
