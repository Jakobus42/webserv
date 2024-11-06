
#include <gtest/gtest.h>
#include "../../include/configfile/ConfigFileParser.hpp"

namespace configfile {

class ConfigFileParserTest : public ::testing::Test {
protected:
    ConfigFileParser instance; // Instance of the class being tested
};



TEST_F(ConfigFileParserTest, DefaultConstructor) {
    ASSERT_NO_THROW(ConfigFileParser new_instance;);
}

} /* namespace configfile */
