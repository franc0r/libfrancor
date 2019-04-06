cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)


enable_testing()

# Image
add_executable(unit-test-image
  src/unit_test_image.cpp
)

target_link_libraries(unit-test-image PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-vision
)

add_test(test-image
  COMMAND unit-test-image
)

# Image filter pipeline
add_executable(unit-test-filter-image
  src/unit_test_filter_image.cpp
)

target_link_libraries(unit-test-filter-image PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-vision
)

add_test(test-filter-image
  COMMAND unit-test-filter-image
)

# Image filter criteria
add_executable(unit-test-filter-image-criteria
  src/unit_test_filter_image_criteria.cpp
)

target_link_libraries(unit-test-filter-image-criteria
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-vision
)

add_test(test-filter-image-criteria
  COMMAND unit-test-filter-image-criteria
)

# vision input output operations
add_executable(unit-test-io
  src/unit_test_io.cpp
)

target_link_libraries(unit-test-io
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-vision
)

add_test(test-vision-io
  COMMAND unit-test-io
)