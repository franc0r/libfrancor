cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

add_executable(unit-test-image
  src/unit_test_image.cpp
)

target_link_libraries(unit-test-image PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-vision
)

enable_testing()

add_test(test-image
  COMMAND unit-test-image
)