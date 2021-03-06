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


# unit test kinematic attribute pack
add_executable(unit-test-kinematic-attribute-pack
  src/unit_test_kinematic_attribute_pack.cpp
)

target_link_libraries(unit-test-kinematic-attribute-pack
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-kinematic-attribute-pack
  COMMAND unit-test-kinematic-attribute-pack
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


# unit test kinematic state vector
add_executable(unit-test-kinematic-state-vector
  src/unit_test_kinematic_state_vector.cpp
)

target_link_libraries(unit-test-kinematic-state-vector
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-kinematic-state-vector
  COMMAND unit-test-kinematic-state-vector
)


# unit test kalman filter
add_executable(unit-test-kalman-filter
  src/unit_test_kalman_filter.cpp
)

target_link_libraries(unit-test-kalman-filter
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-kalman-filter
  COMMAND unit-test-kalman-filter
)


# unit test pose sensor model
add_executable(unit-test-pose-sensor-model
  src/unit_test_pose_sensor_model.cpp
)

target_link_libraries(unit-test-pose-sensor-model
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-mapping
)

add_test(
  NAME test-pose-sensor-model
  COMMAND unit-test-pose-sensor-model
)