
#include <gtest/gtest.h>

#include "http/Connection.hpp"

namespace http {

class ConnectionTest : public ::testing::Test {
	protected:
		Connection instance;  // Instance of the class being tested
};

TEST_F(ConnectionTest, DefaultConstructor) {
	ASSERT_NO_THROW(Connection new_instance;);
}

}  // namespace http
