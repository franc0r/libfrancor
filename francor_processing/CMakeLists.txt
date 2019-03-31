cmake_minimum_required (VERSION 3.7.2)

project(francor-processing VERSION 0.1
                     LANGUAGES CXX)

add_library(${PROJECT_NAME}
  src/data_processing_pipeline.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_processing/include/>
         $<INSTALL_INTERFACE:include/>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC francor-base
  PUBLIC francor-algorithm
  PUBLIC francor-vision
)

add_subdirectory(test)