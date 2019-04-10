cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

add_executable(unit-test-line
  src/unit_test_line.cpp
)

target_link_libraries(unit-test-line PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-line
  COMMAND unit-test-line
)