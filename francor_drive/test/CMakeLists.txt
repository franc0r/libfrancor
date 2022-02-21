cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# -- UNIT TESTS VALUE --
add_executable(unit-test-value
  src/unit_test_value.cpp
)

target_link_libraries(unit-test-value
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-drive
)

add_test(
  NAME test-can-msg
  COMMAND unit-test-value
)

