
#include <gtest/gtest.h>
#include "www/VirtualServerSocket.hpp"

namespace www {

class VirtualServerSocketTest : public ::testing::Test {
protected:
    VirtualServerSocket instance; // Instance of the class being tested
};

TEST_F(VirtualServerSocketTest, DefaultConstructor) {
    ASSERT_NO_THROW(VirtualServerSocket new_instance;);
}

} /* namespace www */
