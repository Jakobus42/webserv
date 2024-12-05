
#include <gtest/gtest.h>

#include "core/RequestHandler.hpp"

namespace core {

class RequestHandlerTest : public ::testing::Test {
	protected:
		RequestHandler instance;  // Instance of the class being tested
};

TEST_F(RequestHandlerTest, DefaultConstructor) {
	ASSERT_NO_THROW(RequestHandler new_instance;);
}

} /* namespace core */
