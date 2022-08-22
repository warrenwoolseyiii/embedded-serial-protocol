cmake_minimum_required(VERSION 3.14)
project(test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories("../src")

file(GLOB sources
  "../src/*.h"
  "../src/*.c"
  "*.c")

add_executable(
  protocol_test
  embedded_protocol_test.cc
  ${sources}
)
target_link_libraries(
  protocol_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(protocol_test)