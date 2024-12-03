
#include <gtest/gtest.h>
#include "../../include/www/ClientSocket.hpp"

namespace www {

class ClientSocketTest : public ::testing::Test {
protected:
    ClientSocket instance; // Instance of the class being tested
};

TEST_F(ClientSocketTest, DefaultConstructor) {
    ASSERT_NO_THROW(ClientSocket new_instance;);
}

} /* namespace www */
