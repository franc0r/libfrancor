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

add_executable(unit-test-logger
  src/unit_test_logger.cpp
)

target_link_libraries(unit-test-logger PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-logger
  COMMAND unit-test-logger
)

# Angle and NormalizedAngle
add_executable(unit-test-angle
  src/unit_test_angle.cpp
)

target_link_libraries(unit-test-angle PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-angle
  COMMAND unit-test-angle
)