cmake_minimum_required (VERSION 3.7.2)

project(francor-algorithm VERSION 0.1
                     LANGUAGES CXX)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
)

find_package(flann REQUIRED)     
find_package(lz4 REQUIRED)

add_library(${PROJECT_NAME} SHARED
  src/geometry_fitting.cpp
  src/ray_caster_2d.cpp
  src/flann_point_pair_estimator.cpp
  src/estimate_transform.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_algorithm/include/>
         $<INSTALL_INTERFACE:include/> 
         ${FLANN_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC francor-base
         ${FLANN_LIBRARIES}
         ${LIBLZ4_LIBRARIES}
)

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT francor-config
        RUNTIME DESTINATION bin/francor
        LIBRARY DESTINATION lib/francor
        ARCHIVE DESTINATION lib/francor)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_algorithm/include/francor_algorithm
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

# install(EXPORT ${PROJECT_NAME}-config DESTINATION lib/cmake)        