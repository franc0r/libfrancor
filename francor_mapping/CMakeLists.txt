cmake_minimum_required (VERSION 3.7.2)

project(francor-mapping VERSION 0.1
                        LANGUAGES CXX)

# find_package(Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} SHARED
  src/algorithm/tsd.cpp
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
)

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT francor-config
        RUNTIME DESTINATION bin/francor
        LIBRARY DESTINATION lib/francor
        ARCHIVE DESTINATION lib/francor)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_base/include/francor_mapping
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
      