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

# Point
add_executable(unit-test-point
  src/unit_test_point.cpp
)

target_link_libraries(unit-test-point PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-point
  COMMAND unit-test-point
)

# Transform
add_executable(unit-test-transform
  src/unit_test_transform.cpp
)

target_link_libraries(unit-test-transform PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-transform
  COMMAND unit-test-transform
)


# Math
add_executable(unit-test-math
  src/unit_test_math.cpp
)

target_link_libraries(unit-test-math PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-math
  COMMAND unit-test-math
)


# Parameter
add_executable(unit-test-parameter
  src/unit_test_parameter.cpp
)

target_link_libraries(unit-test-parameter PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-base
)

add_test(
  NAME test-parameter
  COMMAND unit-test-parameter
)
