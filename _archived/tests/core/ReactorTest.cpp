
#include <gtest/gtest.h>

#include "core/Reactor.hpp"

namespace core {

class ReactorTest : public ::testing::Test {
	protected:
		Reactor instance;  // Instance of the class being tested
};

TEST_F(ReactorTest, DefaultConstructor) {
	ASSERT_NO_THROW(Reactor new_instance;);
}

} /* namespace http */
