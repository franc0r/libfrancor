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

# Ray2d and RayCaster2d
add_executable(unit-test-ray-caster-2d
  src/unit_test_ray_caster_2d.cpp
)

target_include_directories(unit-test-ray-caster-2d
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-ray-caster-2d
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-ray-caster-2d
  COMMAND unit-test-ray-caster-2d
)

# Flann Point Pair Estimator
add_executable(unit-test-flann-point-pair-estimator
  src/unit_test_flann_point_pair_estimator.cpp
)

target_include_directories(unit-test-flann-point-pair-estimator
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-flann-point-pair-estimator
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-flann-point-pair-estimator
  COMMAND unit-test-flann-point-pair-estimator
)

# Transformation Estimation Functions
add_executable(unit-test-estimate-transform
  src/unit_test_estimate_transform.cpp
)

target_include_directories(unit-test-estimate-transform
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-estimate-transform
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-estimate-transform
  COMMAND unit-test-estimate-transform
)

# Transformation Estimation Functions
add_executable(unit-test-icp
  src/unit_test_icp.cpp
)

target_include_directories(unit-test-icp
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-icp
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-icp
  COMMAND unit-test-icp
)


# Shared Memory/Array
add_executable(unit-test-shared-array
  src/unit_test_shared_array.cpp
)

target_link_libraries(unit-test-shared-array PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-algorithm
)

add_test(
  NAME test-shared-array
  COMMAND unit-test-shared-array
)
