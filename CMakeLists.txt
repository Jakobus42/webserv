cmake_minimum_required(VERSION 3.14)
project(webserv_test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/**/*.cpp")

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp" "test/**/*.cpp")
list(REMOVE_ITEM PROJECT_SOURCES "src/main.cpp")

add_executable(
  webserv_test
  ${TEST_SOURCES}
  ${PROJECT_SOURCES} 
)
target_link_libraries(
  webserv_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(webserv_test)