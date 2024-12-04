
#include <gtest/gtest.h>
#include "runtime/EventHandler.hpp"

namespace runtime {

class EventHandlerTest : public ::testing::Test {
protected:
    EventHandler instance; // Instance of the class being tested
};

TEST_F(EventHandlerTest, DefaultConstructor) {
    ASSERT_NO_THROW(EventHandler new_instance;);
}

} /* namespace runtime */
