cmake_minimum_required (VERSION 3.7.2)

project(francor-mapping VERSION 0.1
                        LANGUAGES CXX)

# find_package(Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} SHARED
  src/algorithm/grid.cpp
  src/algorithm/tsd.cpp
  src/algorithm/occupancy_grid.cpp
  src/pipeline_stage_ego_object.cpp
  src/pipeline_stage_occupancy_grid.cpp
  src/pipeline_occupancy_grid.cpp
  src/pipeline_stage_create_pose_measurement.cpp
  src/ego_kalman_filter_model.cpp
  src/occupancy_grid_sensor_model.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_mapping/include/>
         $<INSTALL_INTERFACE:include/francor_mapping>
         $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  francor-base
  francor-algorithm
  francor-processing
  francor-vision
)

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT francor-config
        RUNTIME DESTINATION bin/francor
        LIBRARY DESTINATION lib/francor
        ARCHIVE DESTINATION lib/francor)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_base/include/francor_mapping
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
      