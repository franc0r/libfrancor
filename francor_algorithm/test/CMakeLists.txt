cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# Ransac
add_executable(unit-test-ransac
  src/unit_test_ransac.cpp
)

target_include_directories(unit-test-ransac
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-ransac
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-ransac
  COMMAND unit-test-ransac
)

# Line Ransac
add_executable(unit-test-line-ransac
  src/unit_test_line_ransac.cpp
)

target_include_directories(unit-test-line-ransac
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-line-ransac
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-line-ransac
  COMMAND unit-test-line-ransac
)

# Line Ransac
add_executable(unit-test-line-segment-ransac
  src/unit_test_line_segment_ransac.cpp
)

target_include_directories(unit-test-line-segment-ransac
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-line-segment-ransac
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-line-segment-ransac
  COMMAND unit-test-line-segment-ransac
)