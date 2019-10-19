cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# grid
add_executable(unit-test-grid
  src/unit_test_grid.cpp
)

target_link_libraries(unit-test-grid
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-grid
  COMMAND unit-test-grid
)


# grid tsd
add_executable(unit-test-tsd-algorithm
  src/unit_test_tsd_algorithm.cpp
)

target_link_libraries(unit-test-tsd-algorithm
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-tsd-algorithm
  COMMAND unit-test-tsd-algorithm
)
