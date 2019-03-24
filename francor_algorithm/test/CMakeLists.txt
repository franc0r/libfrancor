cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

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

add_test(test-line-ransac
  COMMAND unit-test-line-ransac
)