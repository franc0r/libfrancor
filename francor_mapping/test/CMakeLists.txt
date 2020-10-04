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


# unit test state attribute vector
add_executable(unit-test-state-attribute-vector
  src/unit_test_state_attribute_vector.cpp
)

target_link_libraries(unit-test-state-attribute-vector
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-state-attribute-vector
  COMMAND unit-test-state-attribute-vector
)


# unit test observation matrix builder
add_executable(unit-test-observation-matrix-builder
  src/unit_test_observation_matrix_builder.cpp
)

target_link_libraries(unit-test-observation-matrix-builder
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-observation-matrix-builder
  COMMAND unit-test-observation-matrix-builder
)