
#include <gtest/gtest.h>
#include "../../include/www/VirtualServer.hpp"

namespace www {

class VirtualServerTest : public ::testing::Test {
protected:
    VirtualServer instance; // Instance of the class being tested
};

TEST_F(VirtualServerTest, DefaultConstructor) {
    ASSERT_NO_THROW(VirtualServer new_instance;);
}

} /* namespace www */
