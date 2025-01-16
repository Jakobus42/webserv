
#include <gtest/gtest.h>

#include "core/VirtualServer.hpp"

namespace http {

	class VirtualServerTest : public ::testing::Test {
		protected:
			VirtualServer instance; // Instance of the class being tested
	};

	TEST_F(VirtualServerTest, DefaultConstructor) {
		ASSERT_NO_THROW(VirtualServer new_instance;);
	}

} // namespace http
