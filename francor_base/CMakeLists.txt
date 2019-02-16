cmake_minimum_required (VERSION 3.12)

project(francor-base VERSION 0.1
                     LANGUAGES CXX)

find_package(Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} INTERFACE
  
)

target_link_libraries(${PROJECT_NAME}
  INTERFACE Eigen3::Eigen
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

#enable_testing()
add_subdirectory(test)