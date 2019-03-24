cmake_minimum_required (VERSION 3.7.2)

project(francor-algorithm VERSION 0.1
                     LANGUAGES CXX)

                
add_library(${PROJECT_NAME}
  src/geometry_fitting.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_algorithm/include/>
         $<INSTALL_INTERFACE:include/>  
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC francor-base
)

add_subdirectory(test)