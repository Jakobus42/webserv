
#include <gtest/gtest.h>
#include "http/Request.hpp"

namespace http {

class RequestTest : public ::testing::Test {
protected:
    Request instance; // Instance of the class being tested
};

TEST_F(RequestTest, DefaultConstructor) {
    ASSERT_NO_THROW(Request new_instance;);
}

} /* namespace http */
