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

add_test(test-data-processing-port
  COMMAND unit-test-data-processing-port
)

# port block
add_test(test-data-processing-pipeline-port-block
  COMMAND unit-test-data-processing-pipeline-port-block
)

add_executable(unit-test-data-processing-pipeline-port-block
  src/unit_test_data_processing_pipeline_port_block.cpp
)

target_link_libraries(unit-test-data-processing-pipeline-port-block
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-processing
)

add_test(test-data-processing-pipeline-port-block
  COMMAND unit-test-data-processing-pipeline-port-block
)

# data processing pipeline stage
add_executable(unit-test-data-processing-pipeline-stage
  src/unit_test_data_processing_pipeline_stage.cpp
)

target_link_libraries(unit-test-data-processing-pipeline-stage
  PRIVATE GTest::GTest
  PRIVATE GTest::Main
  PRIVATE francor-processing
)

enable_testing()

add_test(test-data-processing-pipeline-stage
  COMMAND unit-test-data-processing-pipeline-stage
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

add_test(test-data-processing-pipeline
  COMMAND unit-test-data-processing-pipeline
)