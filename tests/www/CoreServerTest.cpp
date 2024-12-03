
#include "www/CoreServer.hpp"
#include <gtest/gtest.h>

namespace www {

class CoreServerTest : public ::testing::Test {
protected:
  CoreServer instance; // Instance of the class being tested
};

TEST_F(CoreServerTest, DefaultConstructor) {
  ASSERT_NO_THROW(CoreServer new_instance;);
}

} /* namespace www */
