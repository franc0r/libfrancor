cmake_minimum_required (VERSION 3.7.2)

project(francor-algorithm VERSION 0.1
                     LANGUAGES CXX)

# Not good, but I don't know how at moment                     
find_package(Eigen3 REQUIRED NO_MODULE)
                   
add_library(${PROJECT_NAME}
  src/geometry_fitting.cpp
)
message(STATUS "${CMAKE_SOURCE_DIR}/francor_algorithm/include")
target_include_directories(${PROJECT_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}/francor_algorithm/include/
  PUBLIC include
  PUBLIC ${CMAKE_SOURCE_DIR}/francor_base/include/
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC Eigen3::Eigen
  PUBLIC francor-base
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_algorithm/include/>
  PUBLIC $<INSTALL_INTERFACE:include/>
)
