
#include <gtest/gtest.h>
#include "runtime/RequestHandler.hpp"

namespace runtime {

class RequestHandlerTest : public ::testing::Test {
protected:
    RequestHandler instance; // Instance of the class being tested
};

TEST_F(RequestHandlerTest, DefaultConstructor) {
    ASSERT_NO_THROW(RequestHandler new_instance;);
}

} /* namespace runtime */
