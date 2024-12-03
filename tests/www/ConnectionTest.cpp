
#include <gtest/gtest.h>
#include "www/Connection.hpp"

namespace www {

class ConnectionTest : public ::testing::Test {
protected:
    Connection instance; // Instance of the class being tested
};

TEST_F(ConnectionTest, DefaultConstructor) {
    ASSERT_NO_THROW(Connection new_instance;);
}

} /* namespace www */
