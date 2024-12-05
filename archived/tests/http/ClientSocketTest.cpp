
#include <gtest/gtest.h>

#include "http/ClientSocket.hpp"

namespace http {

class ClientSocketTest : public ::testing::Test {
	protected:
		ClientSocket instance;	// Instance of the class being tested
};

TEST_F(ClientSocketTest, DefaultConstructor) {
	ASSERT_NO_THROW(ClientSocket new_instance;);
}

}  // namespace http
