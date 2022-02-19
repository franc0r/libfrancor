cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

add_executable(unit-test-can-msg
  src/unit_test_msg.cpp
)

target_link_libraries(unit-test-can-msg
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-can
)

add_test(
  NAME test-can-msg
  COMMAND unit-test-can-msg
)
