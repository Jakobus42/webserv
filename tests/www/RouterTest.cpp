
#include <gtest/gtest.h>
#include "www/Router.hpp"

namespace www {

class RouterTest : public ::testing::Test {
protected:
    Router instance; // Instance of the class being tested
};

TEST_F(RouterTest, DefaultConstructor) {
    ASSERT_NO_THROW(Router new_instance;);
}

} /* namespace www */
