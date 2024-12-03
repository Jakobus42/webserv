
#include "configfile/Parser.hpp"
#include <gtest/gtest.h>

namespace configfile {

class ConfigFileParserTest : public ::testing::Test {

protected:
  ConfigFileParser instance; // Instance of the class being tested
};

// files configfile_double_brackets, configfile_empty,
// configfile_misplaced_semicolon, configfile_missing_semicolon,
// configfile_only_qoutes, configfile_to_many_closing_brackets
// configfile_unclosed_bracket, configfile_wrong_brackets,
// cofigfile_wrong_command

// compare error message code
/* 	testing::internal::CaptureStderr();
        instance.loadConfigFile(configFileName);
        std::string output = testing::internal::GetCapturedStderr();
        ASSERT_EQ(output, "Error: Configuration file (line 1): Unexpected
   semicolon found\n"); */

TEST_F(ConfigFileParserTest, DefaultConstructor) {
  ASSERT_NO_THROW(ConfigFileParser new_instance;);
}

TEST_F(ConfigFileParserTest, Configfile_Empty) {
  std::string configFileName = "";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_double_brackets) {
  std::string configFileName = "../config/wrong/configfile_double_brackets";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_Empty_file) {
  std::string configFileName = "../config/wrong/configfile_empty";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_misplaced_semicolon) {
  std::string configFileName = "../config/wrong/configfile_misplaced_semicolon";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_missing_semicolon) {
  std::string configFileName = "../config/wrong/configfile_missing_semicolon";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_only_qoutes) {
  std::string configFileName = "../config/wrong/configfile_only_qoutes";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_to_many_closing_brackets) {
  std::string configFileName =
      "../config/wrong/configfile_to_many_closing_brackets";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_unclosed_bracket) {
  std::string configFileName = "../config/wrong/configfile_unclosed_bracket";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_wrong_brackets) {
  std::string configFileName = "../config/wrong/configfile_wrong_brackets";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_wrong_command) {
  std::string configFileName = "../config/wrong/configfile_wrong_command";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 1);
  ASSERT_EQ(instance.getIsLoaded(), 0);
}

TEST_F(ConfigFileParserTest, Configfile_example) {
  std::string configFileName = "../config/configfile_example";
  ASSERT_EQ(instance.loadConfigFile(configFileName), 0);
  // ASSERT_EQ(instance.getIsLoaded(), 1);
}

TEST_F(ConfigFileParserTest, Listen) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"listen", "80"};
    int ret = instance.testFunction("listen", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().port, 80);
    ASSERT_EQ(ret, 0);
  }
  {
    std::vector<std::string> args = {"listen", "1.2.3.4:80"};
    int ret = instance.testFunction("listen", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().port, 80);
    ASSERT_EQ(instance.getConfigData().servers.back().ip_address[0], 1);
    ASSERT_EQ(instance.getConfigData().servers.back().ip_address[1], 2);
    ASSERT_EQ(instance.getConfigData().servers.back().ip_address[2], 3);
    ASSERT_EQ(instance.getConfigData().servers.back().ip_address[3], 4);
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"listen", "65536"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", "-1"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", ""};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", "65", "535"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", "1.1.1:22"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", "1.1.1.1.1:22"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", "256.1.1.1:22"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"listen", "1.1.1.256:22"};
    ASSERT_EQ(instance.testFunction("listen", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, ServerName) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"server_name", "localhost", "localhost2",
                                     "localhost3"};
    int ret = instance.testFunction("server_name", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().server_names[0],
              "localhost");
    ASSERT_EQ(instance.getConfigData().servers.back().server_names[1],
              "localhost2");
    ASSERT_EQ(instance.getConfigData().servers.back().server_names[2],
              "localhost3");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"server_name"};
    ASSERT_EQ(instance.testFunction("server_name", args, lineCount), 1);
  }
  // too many server names
  {
    std::vector<std::string> args = {"server_name"};
    for (int i = 0; i < 1002; i++) {
      args.push_back("localhost");
    }
    ASSERT_EQ(instance.testFunction("server_name", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"server_name", ""};
    ASSERT_EQ(instance.testFunction("server_name", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, ErrorPage) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"error_page", "404", "/404.html"};
    int ret = instance.testFunction("error_page", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().errorPages[404],
              "/404.html");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"error_page"};
    ASSERT_EQ(instance.testFunction("error_page", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"error_page", "404"};
    ASSERT_EQ(instance.testFunction("error_page", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"error_page", "404", ""};
    ASSERT_EQ(instance.testFunction("error_page", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"error_page", "404", "404.html",
                                     "404.html"};
    ASSERT_EQ(instance.testFunction("error_page", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, ClientMaxBodySize) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"client_max_body_size", "100"};
    int ret = instance.testFunction("client_max_body_size", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().max_body_size, 100);
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"client_max_body_size"};
    ASSERT_EQ(instance.testFunction("client_max_body_size", args, lineCount),
              1);
  }
  {
    std::vector<std::string> args = {"client_max_body_size", "100", "100"};
    ASSERT_EQ(instance.testFunction("client_max_body_size", args, lineCount),
              1);
  }
  {
    std::vector<std::string> args = {"client_max_body_size", ""};
    ASSERT_EQ(instance.testFunction("client_max_body_size", args, lineCount),
              1);
  }
  {
    std::vector<std::string> args = {"client_max_body_size",
                                     "2222222222222222222222222"};
    ASSERT_EQ(instance.testFunction("client_max_body_size", args, lineCount),
              1);
  }
}

TEST_F(ConfigFileParserTest, Server) {
  int lineCount = 1;
  // correct usage
  {
    std::vector<std::string> args = {"server"};
    int ret = instance.testFunction("server", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.size(), 1);
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"server", "localhost"};
    ASSERT_EQ(instance.testFunction("server", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, Location) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"location", "/"};
    int ret = instance.testFunction("location", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().locations.size(), 1);
    ASSERT_EQ(instance.getConfigData().servers.back().locations.back().name,
              "/");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"location"};
    ASSERT_EQ(instance.testFunction("location", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"location", "/test", "test"};
    ASSERT_EQ(instance.testFunction("location", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"location", "/test", "test"};
    ASSERT_EQ(instance.testFunction("location", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"location", ""};
    ASSERT_EQ(instance.testFunction("location", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, limitExcept) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  setup = {"location", "/"};
  instance.testFunction("location", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"limit_except", "GET", "POST"};
    int ret = instance.testFunction("limit_except", args, lineCount);
    ASSERT_EQ(
        instance.getConfigData().servers.back().locations.back().methods[0],
        "GET");
    ASSERT_EQ(
        instance.getConfigData().servers.back().locations.back().methods[1],
        "POST");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"limit_except"};
    ASSERT_EQ(instance.testFunction("limit_except", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"limit_except", "GET", "POST", "GET"};
    ASSERT_EQ(instance.testFunction("limit_except", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"limit_except", "Wrong"};
    ASSERT_EQ(instance.testFunction("limit_except", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"limit_except", ""};
    ASSERT_EQ(instance.testFunction("limit_except", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, Return) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  setup = {"location", "/"};
  instance.testFunction("location", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"return", "/test"};
    int ret = instance.testFunction("return", args, lineCount);
    ASSERT_EQ(
        instance.getConfigData().servers.back().locations.back().return_url,
        "/test");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"return"};
    ASSERT_EQ(instance.testFunction("return", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"return", "test", "test"};
    ASSERT_EQ(instance.testFunction("return", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"return", ""};
    ASSERT_EQ(instance.testFunction("return", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, root) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  setup = {"location", "/"};
  instance.testFunction("location", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"root", "/test"};
    int ret = instance.testFunction("root", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().locations.back().root,
              "/test");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"root"};
    ASSERT_EQ(instance.testFunction("root", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"root", "test", "test"};
    ASSERT_EQ(instance.testFunction("root", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"root", ""};
    ASSERT_EQ(instance.testFunction("root", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, Autoindex) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  setup = {"location", "/"};
  instance.testFunction("location", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"autoindex", "on"};
    int ret = instance.testFunction("autoindex", args, lineCount);
    ASSERT_EQ(
        instance.getConfigData().servers.back().locations.back().autoindex,
        true);
    ASSERT_EQ(ret, 0);
  }
  {
    std::vector<std::string> args = {"autoindex", "off"};
    int ret = instance.testFunction("autoindex", args, lineCount);
    ASSERT_EQ(
        instance.getConfigData().servers.back().locations.back().autoindex,
        false);
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"autoindex"};
    ASSERT_EQ(instance.testFunction("autoindex", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"autoindex", "on", "on"};
    ASSERT_EQ(instance.testFunction("autoindex", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"autoindex", ""};
    ASSERT_EQ(instance.testFunction("autoindex", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"autoindex", "awdwa"};
    ASSERT_EQ(instance.testFunction("autoindex", args, lineCount), 1);
  }
}

TEST_F(ConfigFileParserTest, Index) {
  // setup
  int lineCount = 1;
  std::vector<std::string> setup = {"server"};
  instance.testFunction("server", setup, lineCount);
  setup = {"location", "/"};
  instance.testFunction("location", setup, lineCount);
  // correct usage
  {
    std::vector<std::string> args = {"index", "index.html", "index2.html"};
    int ret = instance.testFunction("index", args, lineCount);
    ASSERT_EQ(instance.getConfigData().servers.back().locations.back().index[0],
              "index.html");
    ASSERT_EQ(instance.getConfigData().servers.back().locations.back().index[1],
              "index2.html");
    ASSERT_EQ(ret, 0);
  }
  // incorrect usage
  {
    std::vector<std::string> args = {"index"};
    ASSERT_EQ(instance.testFunction("index", args, lineCount), 1);
  }
  {
    std::vector<std::string> args = {"index", ""};
    ASSERT_EQ(instance.testFunction("index", args, lineCount), 1);
  }
}

} /* namespace configfile */
