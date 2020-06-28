cmake_minimum_required (VERSION 3.7.2)

project(francor-base VERSION 0.1
                     LANGUAGES CXX)

find_package(Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} SHARED
  src/log.cpp
  src/algorithm/point.cpp
  src/algorithm/transform.cpp
  src/algorithm/laser_scan.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_base/include/>
         $<INSTALL_INTERFACE:include/francor_base>
         $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC Eigen3::Eigen
)

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT francor-config
        RUNTIME DESTINATION bin/francor
        LIBRARY DESTINATION lib/francor
        ARCHIVE DESTINATION lib/francor)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_base/include/francor_base
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
      