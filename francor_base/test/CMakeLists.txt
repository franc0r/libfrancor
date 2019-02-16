cmake_minimum_required (VERSION 3.12)

find_package(GTest REQUIRED)

add_executable(unit-test-line
  src/unit_test_line.cpp
)

target_include_directories(unit-test-line
  PRIVATE ${CMAKE_SOURCE_DIR}/francor_base/include
  PRIVATE ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(unit-test-line PRIVATE
#  ${GTEST_BOTH_LIBRARIES}
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  INTERFACE francor_base
  PRIVATE Eigen3::Eigen
)

enable_testing()

add_test(test-line
  COMMAND unit-test-line
)