cmake_minimum_required (VERSION 3.7.2)

project(francor-base VERSION 0.1
                     LANGUAGES CXX)

find_package(Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} INTERFACE
  
)

target_link_libraries(${PROJECT_NAME}
  INTERFACE Eigen3::Eigen
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_base/include/>
  $<INSTALL_INTERFACE:include/>
)

#enable_testing()
add_subdirectory(test)