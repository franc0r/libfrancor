cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

add_executable(unit-test-line
  src/unit_test_line.cpp
)

target_include_directories(unit-test-line
  PRIVATE ${CMAKE_SOURCE_DIR}/francor_base/include
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-line PRIVATE
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  INTERFACE francor-base
  PRIVATE Eigen3::Eigen
)

enable_testing()

add_test(test-line
  COMMAND unit-test-line
)