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


# grid algortihm
add_executable(unit-test-grid-algorithm
  src/unit_test_grid_algorithm.cpp
)

target_link_libraries(unit-test-grid-algorithm
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-grid-algorithm
  COMMAND unit-test-grid-algorithm
)


# occupancy grid algortihm
add_executable(unit-test-occupancy-grid-algorithm
  src/unit_test_occupancy_grid_algorithm.cpp
)

target_link_libraries(unit-test-occupancy-grid-algorithm
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-occupancy-grid-algorithm
  COMMAND unit-test-occupancy-grid-algorithm
)


# validation occupancy register and reconstruction
add_executable(validation-occupancy-register-and-reconstruction
  src/validation_occupancy_register_and_reconstruction.cpp
)

target_link_libraries(validation-occupancy-register-and-reconstruction
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME validation-occupancy-register-and-reconstruction
  COMMAND validation-occupancy-register-and-reconstruction
)