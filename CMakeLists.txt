cmake_minimum_required(VERSION 3.14)
project(webserv_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_FILES tests/*.cpp)
file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_library(webserv_lib STATIC ${SRC_FILES})

add_executable(
  webserv_test
  ${TEST_FILES}
)
target_link_libraries(
  webserv_test
  GTest::gtest_main
  webserv_lib
)

set(GTEST_OUTPUT "" CACHE STRING "" FORCE)

include(GoogleTest)
gtest_discover_tests(webserv_test)

set(ENV{GTEST_OUTPUT} "")