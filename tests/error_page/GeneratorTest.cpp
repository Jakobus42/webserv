
#include <gtest/gtest.h>
#include "../../include/error_page/Generator.hpp"

namespace error_page {

class GeneratorTest : public ::testing::Test {
protected:
    Generator instance; // Instance of the class being tested
};

TEST_F(GeneratorTest, DefaultConstructor) {
    ASSERT_NO_THROW(Generator new_instance;);
}

} /* namespace error_page */
