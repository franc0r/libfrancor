cmake_minimum_required (VERSION 3.7.2)

find_package(GTest REQUIRED)

# port
add_executable(unit-test-data-processing-port
  src/unit_test_data_processing_port.cpp
)

target_link_libraries(unit-test-data-processing-port
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-processing
)

add_test(
  NAME test-data-processing-port
  COMMAND unit-test-data-processing-port
)

# data processing pipeline
add_executable(unit-test-data-processing-pipeline
  src/unit_test_data_processing_pipeline.cpp
)

target_link_libraries(unit-test-data-processing-pipeline
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-processing
)

enable_testing()

add_test(
  NAME test-data-processing-pipeline
  COMMAND unit-test-data-processing-pipeline
)