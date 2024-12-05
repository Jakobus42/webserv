
#include <gtest/gtest.h>

#include "http/ServerSocket.hpp"

namespace http {

class ServerSocketTest : public ::testing::Test {
	protected:
		ServerSocket instance;	// Instance of the class being tested
};

TEST_F(ServerSocketTest, DefaultConstructor) {
	ASSERT_NO_THROW(ServerSocket new_instance;);
}

}  // namespace http
